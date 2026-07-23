#pragma once

#include "input/GCursorControl.hpp"

#include <functional>
#include <utility>

namespace gentau {
class GCursorControlBackend
{
  public:
	using LockStateChangedCallback = std::function<void(GCursorControl::LockState)>;

	explicit GCursorControlBackend(LockStateChangedCallback lockStateChanged)
	  : _lockStateChanged(std::move(lockStateChanged))
	{}

	virtual ~GCursorControlBackend() = default;

	virtual bool lock()   = 0;
	virtual void unlock() = 0;

	virtual bool isLockSupported() const noexcept = 0;

	virtual QPointF captureDeltaMovement(GCursorControl::MovementMode mode) noexcept = 0;

  protected:
	void notifyLockStateChanged(GCursorControl::LockState state)
	{
		if (_lockStateChanged) { _lockStateChanged(state); }
	}

  private:
	LockStateChangedCallback _lockStateChanged;
};
}  // namespace gentau
