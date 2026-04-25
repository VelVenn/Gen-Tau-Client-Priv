#include "img_trans/net/TReassembly.hpp"

#include "img_trans/vid_render/TFramePool.hpp"

#include "utils/TLog.hpp"

#include "conf/version.hpp"

#include <arpa/inet.h>

#include <chrono>
#include <cstring>
#include <string_view>

#define T_LOG_TAG_IMG "[Reassembler] "

using namespace std;
using namespace std::literals;

namespace gentau {
TReassembly::TReassembly(TVidRender::SharedPtr _renderer) : renderer(std::move(_renderer))
{
	if constexpr (!conf::TDebugMode) {
		if (renderer == nullptr) {
			constexpr auto errMsg = "Renderer cannot be nullptr"sv;
			tImgTransLogError("{}", errMsg);
			throw std::invalid_argument(errMsg.data());
		}
	} else {
		if (!renderer) {
			tImgTransLogWarn(
				"Renderer is nullptr, this is allowed in Debug build for testing purposes, but may "
				"cause some features to not work properly. Use with caution."
			);
		}
	}
}

auto TReassembly::Header::fromLiInplace(span<const u8> data) noexcept -> const Header*
{
	if (data.size() < sizeof(Header)) { return nullptr; }
	return reinterpret_cast<const Header*>(data.data());
}

auto TReassembly::Header::fromBiInplace(span<u8> data) noexcept -> const Header*
{
	if (data.size() < sizeof(Header)) { return nullptr; }
	auto header      = reinterpret_cast<Header*>(data.data());
	header->frameIdx = ntohs(header->frameIdx);
	header->secIdx   = ntohs(header->secIdx);
	header->frameLen = ntohl(header->frameLen);

	return header;
}

auto TReassembly::Header::fromBi(span<const u8> data) noexcept -> optional<Header>
{
	if (data.size() < sizeof(Header)) { return std::nullopt; }
	Header header;

	auto liHeaderPtr = fromLiInplace(data);
	if (!liHeaderPtr) { return std::nullopt; }

	header.frameIdx = ntohs(liHeaderPtr->frameIdx);
	header.secIdx   = ntohs(liHeaderPtr->secIdx);
	header.frameLen = ntohl(liHeaderPtr->frameLen);

	return header;
}

bool TReassembly::ReassemblingFrame::fill(std::span<u8> packet, const Header* header)
{
	if (!isOccupied() || isComplete()) { return false; }

	if (packet.empty() || header == nullptr) { return false; }

	auto packetLen = static_cast<u32>(packet.size());

	u16 secIdx      = header->secIdx;
	u32 offset      = secIdx * maxPayloadSize;
	u32 payloadSize = packetLen < sizeof(Header) ? 0 : packetLen - sizeof(Header);
	u32 frameLen    = frameSlot->getDataLen();
	u8* destPtr     = frameSlot->data();

	// tImgTransLogDebug("secIdx {} | offset {} | payloadSize {} | frameLen {}", secIdx, offset, payloadSize, frameLen);

	if (secIdx >= receivedSecs.size() || payloadSize == 0) { return false; }

	if (receivedSecs.test(secIdx)) { return false; }

	// Avoid the padding bytes
	if (offset + payloadSize > frameLen) { payloadSize = frameLen - offset; }

	if (!destPtr) { return false; }

	memcpy(destPtr + offset, packet.data() + sizeof(Header), payloadSize);
	receivedSecs.set(secIdx);
	curLen += payloadSize;

	return true;
}

auto TReassembly::findReAsmSlot(u16 idx) -> ReassemblingFrame*
{
	ReassemblingFrame* firstEmpty  = nullptr;
	ReassemblingFrame* oldestSmall = nullptr;
	ReassemblingFrame* oldestLarge = nullptr;

	auto minTimeSmall = TimePoint::max();
	auto minTimeLarge = TimePoint::max();

	for (auto& frame : rFrames) {
		if (frame.isOccupied() && frame.frameIdx == idx) { return &frame; }

		if (!frame.isOccupied()) {
			if (!firstEmpty) { firstEmpty = &frame; }
		} else {
			bool isLarge = frame.frameSlot->getDataLen() >= bigFrameThres;

			if (isLarge) {
				if (frame.asmStartTime < minTimeLarge) {
					minTimeLarge = frame.asmStartTime;
					oldestLarge  = &frame;
				}
			} else {
				if (frame.asmStartTime < minTimeSmall) {
					minTimeSmall = frame.asmStartTime;
					oldestSmall  = &frame;
				}
			}
		}
	}

	if (firstEmpty) { return firstEmpty; }

	// 如果网络状况极差，可能需要加入额外的 Header::isAfter 判断，但这种判断可能会导致僵尸帧无法被
	// 正常清理，这里暂时保持抢占式清理策略即可
	if (oldestSmall) {
		tImgTransLogWarn("Dropping oldest SMALL frame: {}", oldestSmall->frameIdx);
		oldestSmall->clear();
		return oldestSmall;
	}

	if (oldestLarge) {
		tImgTransLogWarn("Dropping oldest LARGE frame: {}", oldestLarge->frameIdx);
		oldestLarge->clear();
		return oldestLarge;
	}

	return nullptr;
}

void TReassembly::onPacketRecv(std::span<u8> packetData, TRecvPasskey)
{
	auto now = chrono::steady_clock::now();
	if (synced.load() && now - lastSyncedTime.load() > syncTimeout) {
		tImgTransLogWarn("Sync timeout detected on recieving packet.");
		synced.store(false);
	}

	if (packetData.empty() || packetData.size() < sizeof(Header)) {
		tImgTransLogWarn("Received packet too small to contain valid header, ignoring.");
		return;
	}

	auto header = [&packetData] {
		if constexpr (conf::TVTHeaderFromBi) {
			return Header::fromBiInplace(packetData);
		} else {
			return Header::fromLiInplace(packetData);
		}
	}();

	if (!header) {
		tImgTransLogWarn("Received packet with invalid header, ignoring.");
		return;
	}

	if (header->frameLen > TFramePool::slotLen) {
		tImgTransLogWarn(
			"Received packet with frame length {} exceeding slot capacity, ignoring.",
			header->frameLen
		);
		return;
	}

	auto frameIdxDiff = Header::diff(header->frameIdx, lastPushedIdx.load());

	if (synced.load() && frameIdxDiff < minFrameIdxDiff) {
		tImgTransLogWarn(
			"Received abnormally old frame: {} (sec {}), last pushed frame: {}, considering it as "
			"new session start.",
			header->frameIdx,
			header->secIdx,
			lastPushedIdx.load()
		);
		synced.store(false);
	}

	if (synced.load() && frameIdxDiff <= 0) {
		return;
	}  // Drop likely normal duplicate or out-of-order packet

	if (!synced.load()) {
		synced.store(true);

		// set to one before current. It's ok to overflow.
		lastPushedIdx.store(header->frameIdx - 1);

		for (auto& frame : rFrames) { frame.clear(); }  // Clear all reassembly frames when de-sync.

		tImgTransLogDebug("Session synced at frame {}, sec {}.", header->frameIdx, header->secIdx);
	}
	lastSyncedTime.store(now);

	auto rSlot = findReAsmSlot(header->frameIdx);
	if (!rSlot) [[unlikely]] {
		tImgTransLogWarn(
			"No available reassembly slot for frame {}, dropping packet.", header->frameIdx
		);
		return;
	}

	if (!rSlot->isOccupied()) {
		auto frameDataOpt = renderer->acquireFrameSlot({});
		if (!frameDataOpt.has_value()) { return; }

		rSlot->frameSlot = std::move(frameDataOpt).value();
		rSlot->frameSlot->setDataLen(header->frameLen);

		rSlot->frameIdx     = header->frameIdx;
		rSlot->asmStartTime = now;
	}

	if (rSlot->fill(packetData, header)) {
		if (rSlot->isComplete()) {
			renderer->tryPushFrame(rSlot->steal(), {});
			lastPushedIdx.store(header->frameIdx);

			rSlot->clear();  // Reset metadata, the actual frame has been moved.

			// for (auto& frame : rFrames) {
			// 	if ((frame.isOccupied() && Header::isBefore(frame.frameIdx, header->frameIdx)) ||
			// 		frame.frameIdx == header->frameIdx) {
			// 		frame.clear();
			// 	}
			// }

			// Note: 这里的激进清理可能会导致一些边缘情况的帧被过早丢弃，暂时先不启用。
			// 找重组槽位时的抢占式清理与重组超时检查已经能够在大多数情况下保证僵尸帧不
			// 会过多积累，且不会过早丢弃正常帧。
		}
	}
};

void TReassembly::ReAsmSlotScan(TRecvPasskey)
{
	auto now = chrono::steady_clock::now();

	if (synced.load() && now - lastSyncedTime.load() > syncTimeout) {
		tImgTransLogWarn("Sync timeout detected on reassembling frame slot scan.");
		synced.store(false);
	}

	for (auto& frame : rFrames) {
		// 检查重组超时的帧
		if (frame.isOccupied() && now - frame.asmStartTime > reassembleTimeout) {
			if (pushIncompleteAllowed() && frame.getCompleteRate() >= minFrameCompleteRate) {
				if (Header::isAfter(frame.frameIdx, lastPushedIdx.load())) {
					renderer->tryPushFrame(frame.steal(), {});
					lastPushedIdx.store(frame.frameIdx);
				}

				// tImgTransLogTrace("Trying to push corrupted frame...");

				// 能在这里扫描的都是通过 onPacketRecv 的 frameIdxDiff 的严判的，所有这里只用检查
				// 是否比 lastPushedIdx 大即可，防止出现 push 了一个更旧的帧导致画面出现回退。
			}

			// 无论是否推送，都清理掉这个重组槽位，防止僵尸帧过多积累导致后续帧无法重组。
			frame.clear();
		}
	}
}
}  // namespace gentau