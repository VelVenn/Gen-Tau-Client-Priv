#pragma once

#include "utils/TTypeRedef.hpp"

#include "concurrentqueue.h"

#include <array>
#include <cstdint>
#include <cstring>
#include <memory>
#include <new>
#include <optional>

namespace gentau {
class [[gnu::aligned(64)]] TFramePool
{
  public:
	static constexpr u32 poolSize = 10;
	static constexpr u32 slotLen  = 2 * 1024 * 1024;  // 2 MiB per slot

  public:
	using SharedPtr   = std::shared_ptr<TFramePool>;
	using Frame       = std::array<u8, slotLen>;
	using Pool        = std::array<Frame, poolSize>;
	using FreeIdxList = moodycamel::ConcurrentQueue<u32>;

  public:
	class FrameData
	{
	  private:
		TFramePool* pool     = nullptr;
		Frame*      frame    = nullptr;
		u32         idx      = UINT32_MAX;
		u32         frameLen = 0;

	  public:
		u8* data() noexcept
		{
			if (!isValid()) { return nullptr; }
			return frame->data();
		}
		u32 index() const noexcept { return idx; }

		u32  getDataLen() const noexcept { return frameLen; }
		void setDataLen(u32 len) noexcept { frameLen = len; }

	  public:
		bool isValid() const noexcept
		{
			return pool != nullptr && frame != nullptr && idx < poolSize;
		}

	  public:
		FrameData(TFramePool* _pool, Frame* _frame, u32 _idx) :
			pool(_pool),
			frame(_frame),
			idx(_idx) {};

		~FrameData()
		{
			if (isValid()) { pool->restore(idx); }
		}

		FrameData(FrameData&& other) :
			pool(other.pool),
			frame(other.frame),
			idx(other.idx),
			frameLen(other.frameLen)
		{
			other.pool     = nullptr;
			other.frame    = nullptr;
			other.idx      = UINT32_MAX;
			other.frameLen = 0;
		}

		FrameData& operator=(FrameData&& other)
		{
			if (this != &other) {
				if (isValid()) { pool->restore(idx); }

				pool     = other.pool;
				frame    = other.frame;
				idx      = other.idx;
				frameLen = other.frameLen;

				other.pool     = nullptr;
				other.frame    = nullptr;
				other.idx      = UINT32_MAX;
				other.frameLen = 0;
			}
			return *this;
		}

		FrameData(const FrameData&)            = delete;
		FrameData& operator=(const FrameData&) = delete;
	};

  private:
	Pool        poolData;
	FreeIdxList freeIdxList;

  public:
	std::optional<FrameData> acquire()
	{
		u32 idx;
		if (!freeIdxList.try_dequeue(idx)) { return std::nullopt; }
		return FrameData(this, &poolData[idx], idx);
	}

  private:
	bool restore(u32 idx)
	{
		if (idx >= poolSize) { return false; }

		return freeIdxList.enqueue(idx);
	}

  public:
	TFramePool() : poolData(), freeIdxList(poolSize)
	{
		std::memset(poolData.data(), 0, sizeof(poolData));

		for (u32 i = 0; i < poolSize; i++) {
			if (!freeIdxList.enqueue(i)) { throw std::bad_alloc{}; }
		}
	}

	~TFramePool() = default;

	[[nodiscard("Should not ignore the created TFramePool::SharedPtr")]] static SharedPtr create()
	{
		return std::make_shared<TFramePool>();
	}

	TFramePool(const TFramePool&)            = delete;  // Forbid copy or move
	TFramePool& operator=(const TFramePool&) = delete;
	TFramePool(TFramePool&&)                 = delete;
	TFramePool& operator=(TFramePool&&)      = delete;
};
}  // namespace gentau