#pragma once

#include "input/GCursorControl.hpp"

namespace gentau {
class GCursorControlBackend
{
  public:
	virtual ~GCursorControlBackend() = default;

	virtual void lock()   = 0;
	virtual void unlock() = 0;

	virtual QPointF getDeltaMovement(GCursorControl::MovementMode mode) noexcept = 0;
};
}  // namespace gentau
