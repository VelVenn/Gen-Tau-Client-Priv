#include "img_trans/net/TRecv.hpp"

#include "utils/TLog.hpp"

#include <cerrno>

#include <array>
#include <chrono>
#include <future>
#include <stop_token>
#include <string_view>
#include <system_error>
#include <thread>

#define T_LOG_TAG_IMG "[UDP Receiver] "

using namespace std;

namespace gentau {
void TRecv::stop()
{
	if (recvThread.joinable()) {
		recvThread.request_stop();
		recvThread.join();
	}
}

void TRecv::stopAsync()
{
	recvThread.request_stop();
}

int TRecv::start()
{
	if (!isBound()) {
		tImgTransLogError("Cannot start receiving thread: socket is not bound");
		return EBADF;  // Bad file descriptor
	}

	if (recvThread.joinable()) {
		tImgTransLogInfo("Receiving thread is already running");
		return 0;  // Already running, consider it a success
	}

	promise<i32> threadErrPassThru;
	auto         passThruFuture = threadErrPassThru.get_future();

	recvThread = jthread([this,
						  passThru = std::move(threadErrPassThru)](stop_token sToken) mutable {
		if (::setsockopt(updSock, SOL_SOCKET, SO_RCVBUF, &kRecvBufferSize, sizeof(i32)) < 0) {
			tImgTransLogError(
				"Failed to set socket kernel receive buffer size, error: {}",
				error_code(errno, system_category()).message()
			);
			passThru.set_value(errno);
			return;
		}

		if (::setsockopt(updSock, SOL_SOCKET, SO_RCVTIMEO, &kRecvTimeout, sizeof(timeval)) < 0) {
			tImgTransLogError(
				"Failed to set socket receive timeout, error: {}",
				error_code(errno, system_category()).message()
			);
			passThru.set_value(errno);
			return;
		}

		passThru.set_value(0);

		struct [[gnu::aligned(64)]] RecvBuf
		{
			array<u8, MTU_LEN> packet;

			auto data() { return packet.data(); }

			RecvBuf() { memset(packet.data(), 0, MTU_LEN); }
		} recvBuffer;

		i32           ENOMEM_count = 0;
		constexpr i32 ENOMEM_THRES = 5;

		auto           lastReAsmScanTime = chrono::steady_clock::now();
		constexpr auto reAsmScanInv      = 5ms;

		while (!sToken.stop_requested()) {
			auto now = chrono::steady_clock::now();
			if (now - lastReAsmScanTime > reAsmScanInv) {
				reassembler->ReAsmSlotScan({});
				lastReAsmScanTime = now;
			}

			auto ret = ::recv(updSock, recvBuffer.data(), MTU_LEN, 0);

			if (ret > 0) {
				ENOMEM_count = 0;  // Reset ENOMEM counter on successful receive

				lastRecvTime.store(chrono::steady_clock::now());

				reassembler->onPacketRecv(std::span(recvBuffer.packet).subspan(0, ret), {});
			} else if (ret == 0) [[unlikely]] {
				ENOMEM_count = 0;  // ret == 0 indicates zero-length packet in UDP (DGRAM sock)

				continue;
			} else {
				if (errno == EAGAIN || errno == EWOULDBLOCK) {
					continue;  // Timeout, just try again
				}

				if (errno == ENOMEM) {
					ENOMEM_count++;
					tImgTransLogWarn(
						"Receive failed with ENOMEM (kernel socket buffer out of memory), "
						"consecutive count: {}.",
						ENOMEM_count
					);

					if (ENOMEM_count > ENOMEM_THRES) {
						onRecvError(errno);
						tImgTransLogError(
							"Receive failed with ENOMEM {} times in a row. Stopping recieve "
							"thread.",
							ENOMEM_count
						);
						break;
					}

					this_thread::sleep_for(chrono::milliseconds(ENOMEM_count));
					continue;
				}

				if (errno == EINTR) {
					tImgTransLogWarn("Recieve interrupted by a system signal");
					continue;  // Interrupted by signal, just try again
				}

				if (errno == ECONNREFUSED || errno == ENOTCONN) [[unlikely]] {
					tImgTransLogWarn(
						"Recieve failed with error: {}, ignoring this",
						error_code(errno, system_category()).message()
					);
					continue;  // These errors should not happen as there is no connection and send at all
				}

				tImgTransLogError(
					"Receive failed with error: {}. Stopping receive thread.",
					error_code(errno, system_category()).message()
				);
				onRecvError(errno);
				break;
			}
		}

		tImgTransLogTrace("UDP Receive thread stopped");
	});

	auto threadErr = passThruFuture.get();

	return threadErr;
}

i32 TRecv::bindV4(u16 port, const char* ip)
{
	stop();

	updSock.closeSock();
	auto newSockFd = ::socket(AF_INET, SOCK_DGRAM, 0);

	if (newSockFd < 0) {
		const int errorCode = errno;
		tImgTransLogError(
			"Failed to create new socket, error: {}",
			error_code(errorCode, system_category()).message()
		);
		return errorCode;
	}

	sockaddr_in newAddr{};

	auto v4Addr = V4Addr::create(ip, port);
	if (!v4Addr.has_value()) {
		tImgTransLogError("Invalid IP address: {}:{}", ip, port);
		::close(newSockFd);
		return EINVAL;  // Invalid argument
	}

	newAddr.sin_family      = AF_INET;
	newAddr.sin_addr.s_addr = v4Addr->ip;
	newAddr.sin_port        = htons(v4Addr->port);

	if (::bind(newSockFd, reinterpret_cast<sockaddr*>(&newAddr), sizeof(newAddr)) < 0) {
		const int errorCode = errno;
		tImgTransLogError(
			"Failed to bind socket to ip: {}, error: {}",
			v4Addr->toString(),
			error_code(errorCode, system_category()).message()
		);
		::close(newSockFd);
		return errorCode;
	}

	socklen_t newAddrLen = sizeof(newAddr);

	if (::getsockname(newSockFd, reinterpret_cast<sockaddr*>(&newAddr), &newAddrLen) < 0) {
		const int errorCode = errno;
		tImgTransLogError(
			"Failed to get socket name, error: {}",
			error_code(errorCode, system_category()).message()
		);
		::close(newSockFd);
		return errorCode;
	}

	updSock    = newSockFd;
	listenAddr = newAddr;

	v4Addr->port = ntohs(newAddr.sin_port);  // Update port in case it was 0 (ephemeral port)

	tImgTransLogInfo("New socket created, bound to {}", v4Addr->toString());

	return 0;
}

TRecv::TRecv(TReassembly::SharedPtr _reassembler, u16 _port, const char* _ip) :
	reassembler(std::move(_reassembler))
{
	if (!reassembler) {
		if constexpr (!conf::TDebugMode) {
			constexpr auto errMsg = "Reassembler cannot be nullptr"sv;
			tImgTransLogError("{}", errMsg);
			throw std::invalid_argument(errMsg.data());
		} else {
			tImgTransLogWarn(
				"Reassembler is nullptr, this is allowed in Debug build for testing purposes, but "
				"may cause some features to not work properly. Use with caution."
			);
		}
	}

	auto bindResult = bindV4(_port, _ip);
	if (bindResult != 0) {
		tImgTransLogError(
			"Failed to bind to {}:{}, error: {}",
			_ip,
			_port,
			error_code(bindResult, system_category()).message()
		);
	}
}

TRecv::~TRecv()
{
	tImgTransLogDebug("TRecv released, closing socket...");
}

}  // namespace gentau
