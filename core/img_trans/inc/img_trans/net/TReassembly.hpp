#pragma once

#include "img_trans/vid_render/TFramePool.hpp"
#include "img_trans/vid_render/TVidRender.hpp"

#include "utils/TTypeRedef.hpp"

#include <array>
#include <atomic>
#include <bitset>
#include <chrono>
#include <memory>
#include <optional>
#include <span>

namespace gentau {
constexpr u64 MTU_LEN = 1400;  // 1400 B

class TRecv;

class TRecvPasskey
{
	friend class TRecv;
	TRecvPasskey() = default;
};

class TReassembly : public std::enable_shared_from_this<TReassembly>
{
  public:
	using SharedPtr = std::shared_ptr<TReassembly>;
	using TimePoint = std::chrono::steady_clock::time_point;

  public:
	/**
     * @brief Header struct of the UDP packet according to the RM Comm. Protocol 
     *
     * @note Size is 8 bytes, no alignment, might fail to construct on some
	 *       specific CPU archs.
     * Ref: https://qingflow.com/appView/c5rf6rkkbs02/shareView/c5rf6slgbs02?applyId=693818572
     */
	struct [[gnu::packed]] Header
	{
		u16 frameIdx;
		u16 secIdx;
		u32 frameLen;

		/**
		 * @brief Calculate the difference between two frame indices, considering wrap-around.
		 * @return The signed difference between idx_a and idx_b. Positive if idx_a is after 
		 *          idx_b, negative if idx_a is before idx_b, zero if they are the same.
		 */
		static constexpr i16 diff(u16 idx_a, u16 idx_b) noexcept
		{
			return static_cast<i16>(idx_a - idx_b);
		}

		/**
		 * @brief Check if idx_a is after idx_b, considering wrap-around.
		 */
		static constexpr bool isAfter(u16 idx_a, u16 idx_b) noexcept
		{
			return diff(idx_a, idx_b) > 0;
		}

		/**
		 * @brief Check if idx_a is before idx_b, considering wrap-around.
		 */
		static constexpr bool isBefore(u16 idx_a, u16 idx_b) noexcept
		{
			return diff(idx_a, idx_b) < 0;
		}

		/**
         * @brief Parse a raw buffer into a Header pointer inplace, interprete from little-endian format.
         *
         * @param data The raw buffer to parse.
         * @return The parsed Header pointer, or nullptr if the buffer is too small.
         * @note No memory allocation or ownership transfer is performed.
         */
		[[nodiscard("The parsed header pointer should not be ignored")]]
		static const Header* fromLiInplace(std::span<const u8> data) noexcept;

		/**
		 * @brief Parse a raw buffer into a Header pointer inplace, interprete from big-endian format.
		 *
		 * @param data The raw buffer to parse.
		 * @return The parsed Header pointer, or nullptr if the buffer is too small.
		 * @note No memory allocation or ownership transfer is performed.
		 */
		[[nodiscard("The parsed header pointer should not be ignored")]]
		static const Header* fromBiInplace(std::span<u8> data) noexcept;

		/**
		 * @brief Parse a raw buffer into a Header object, interprete from big-endian format.
		 *
		 * @param data The raw buffer to parse.
		 * @return The parsed Header object, or std::nullopt if the buffer is too small.
		 */
		static std::optional<Header> fromBi(std::span<const u8> data) noexcept;
	};
	static_assert(sizeof(Header) == 8, "Header size must be 8 bytes");

  public:
	static constexpr u32 maxReAsmSlots        = 5;
	static constexpr u32 maxPayloadSize       = MTU_LEN - sizeof(Header);
	static constexpr u32 maxSecPerFrame       = 1536;   // 1536 = 64 * 24, 1536 * 1392 ≈ 2.04 MiB
	static constexpr u32 bigFrameThres        = 5000;   // 5 KB
	static constexpr i16 minFrameIdxDiff      = -180;   // About 3 seconds, assuming 60 FPS
	static constexpr f32 minFrameCompleteRate = 0.95f;  // Minimum receive data ratio to tolerate

	// About 3.5 frames at 60 FPS
	static constexpr std::chrono::milliseconds reassembleTimeout{ 60 };
	static constexpr std::chrono::milliseconds syncTimeout{ 1000 };

  private:
	struct ReassemblingFrame
	{
		std::optional<TFramePool::FrameData> frameSlot    = std::nullopt;
		u16                                  frameIdx     = 0;
		u32                                  curLen       = 0;
		TimePoint                            asmStartTime = TimePoint::min();
		std::bitset<maxSecPerFrame>          receivedSecs;  // bitmap is based on uint64_t array

		void clear() noexcept
		{
			if (frameSlot.has_value()) { frameSlot.reset(); }
			frameIdx     = 0;
			curLen       = 0;
			asmStartTime = TimePoint::min();
			receivedSecs.reset();
		}

		TFramePool::FrameData steal()
		{
			if (!frameSlot.has_value()) {
				return TFramePool::FrameData(nullptr, nullptr, UINT32_MAX);
			}

			auto data = std::move(frameSlot).value();
			frameSlot.reset();

			return data;
		}

		bool isOccupied() const noexcept
		{
			if (!frameSlot.has_value()) { return false; }

			return frameSlot.value().isValid();
		}

		bool isComplete() const noexcept
		{
			if (!frameSlot.has_value()) { return false; }

			if (!frameSlot.value().isValid()) { return false; }

			return curLen == frameSlot.value().getDataLen();
		}

		f32 getCompleteRate() const noexcept
		{
			if (!frameSlot.has_value()) { return 0.0f; }

			if (!frameSlot.value().isValid()) { return 0.0f; }

			if (curLen > frameSlot->getDataLen()) { return 0.0f; }

			return static_cast<f32>(curLen) / static_cast<f32>(frameSlot->getDataLen());
		}

		bool fill(std::span<u8> packet, const Header* header);
	};

  private:
	const TVidRender::SharedPtr                  renderer;
	std::array<ReassemblingFrame, maxReAsmSlots> rFrames;

  private:
	std::atomic<TimePoint> lastSyncedTime      = TimePoint::min();
	std::atomic<u16>       lastPushedIdx       = 0;
	std::atomic<bool>      synced              = false;
	std::atomic<bool>      allowPushIncomplete = false;

  public:
	/**
	 * @brief 获取上一次网络连接同步成功（即收到有效包）的时间点。若未曾成功同步过，返回 TimePoint::min()。
	 * @note 多线程安全。
	 */
	TimePoint getLastSyncedTime() const noexcept { return lastSyncedTime.load(); }

	/**
	 * @brief 获取上一次推送到渲染管线的帧索引。若从未推送过任何帧，返回 0。
	 * @note 多线程安全。
	 */
	u16 getLastPushedIdx() const noexcept { return lastPushedIdx.load(); }

	/**
	 * @brief 检查当前是否处于网络连接同步状态（即是否持续收到有效包）。
	 * @note 多线程安全。
	 */
	bool isSynced() const noexcept { return synced.load(); }

	/**
	 * @brief 检查是否允许推送重组未完成的帧到渲染管线。该设置默认为 false。
	 * @note 多线程安全。
	 */
	bool pushIncompleteAllowed() const noexcept { return allowPushIncomplete.load(); }

	/**
	 * @brief 设置是否允许推送重组未完成的帧到渲染管线。该设置默认为 false。
	 * @note 多线程安全。开启后有较大概率导致花屏，但可以保证画面最大程度的不出现卡顿。
	 *       仅建议在网络状况极差且对渲染完整性要求不高的情况下开启。请务必谨慎使用该设置。
	 * @note 该 API 目前仅在 nVidia decoder 测试过，其他硬解码器的表现可能会有较大差异，
	 *       有可能会导致严重问题。软解码器由于本身的容错机制，开启该设置一般不会有明显的负面
	 *       影响，但不做绝对保证。
	 */
	void allowPushIncompleteFrames(bool allow) noexcept { allowPushIncomplete.store(allow); }

  public:
	/**
	 * @brief 处理接收到的原始数据包。
	 * @param packetData 接收到的包含协议头部的原始数据包内容，除此之外不能包含任何额外的填充字节。
	 * @note 该方法仅能在 TRecv 类内部被正常调用，其他地方调用此方法将导致编译错误。该方法当且仅当
	 *       存在单一调用者时才是线程安全的，请勿在多个线程中并发调用此方法。
	 */
	void onPacketRecv(std::span<u8> packetData, TRecvPasskey);

	/**
	 * @brief 检查同步状态。扫描当前正在重组的帧，检查是否有重组超时的帧，并进行相应的处理。
	 * @note 该方法仅能在 TRecv 类内部被正常调用，其他地方调用此方法将导致编译错误。该方法当且仅当
	 *       存在单一调用者时才是线程安全的，请勿在多个线程中并发调用此方法。
	 */
	void ReAsmSlotScan(TRecvPasskey);

  private:
	ReassemblingFrame* findReAsmSlot(u16 frameIdx);

  public:
	/**
	 * @brief constructor of TReassembly.
	 * @throw std::invalid_argument if the provided TVidRender::SharedPtr is nullptr
	 *        in Non-Debug build.
	 */
	explicit TReassembly(TVidRender::SharedPtr _renderer);

	/**
	 * @brief create a shared pointer to TReassembly instance. 
	 * @throw std::invalid_argument if the provided TVidRender::SharedPtr is nullptr
	 *        in Non-Debug build.
	 */
	[[nodiscard("Should not ignored the created TReassembly::SharedPtr")]] static SharedPtr create(
		TVidRender::SharedPtr _renderer
	)
	{
		return std::make_shared<TReassembly>(std::move(_renderer));
	}
	~TReassembly() = default;

	TReassembly()                              = delete;  // Forbid default construction
	TReassembly(const TReassembly&)            = delete;  // Forbid copy or move
	TReassembly& operator=(const TReassembly&) = delete;
	TReassembly(TReassembly&&)                 = delete;
	TReassembly&& operator=(TReassembly&&)     = delete;
};
}  // namespace gentau