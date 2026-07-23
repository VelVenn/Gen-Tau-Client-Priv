#include "input/GCursorControl.hpp"

#include "native/GCursorControlBackend.hpp"

#include <QGuiApplication>
#include <QWindow>

#ifdef Q_OS_LINUX
#include "native/wayland/GWaylandCursorControl.hpp"
#endif

namespace gentau {
GCursorControl::GCursorControl(QWindow* window, QObject* parent) : QObject(parent)
{
#ifdef Q_OS_LINUX
	if (window && QGuiApplication::platformName().startsWith("wayland", Qt::CaseInsensitive)) {
		_backend = std::make_unique<GWaylandCursorControl>(
		  window,
		  [this](LockState state) { updateLockState(state); });
		if (_backend->isLockSupported()) {
			_lockState.store(LockState::Unlocked, std::memory_order_release);
		}
	}
#else
	Q_UNUSED(window);
#endif
}

GCursorControl::~GCursorControl() = default;

void GCursorControl::lock()
{
	if (!_backend) { return; }
	if (_backend->lock()) { updateLockState(LockState::Pending); }
}

void GCursorControl::unlock()
{
	if (!_backend || lockState() == LockState::Unsupported) { return; }

	_backend->unlock();
	updateLockState(LockState::Unlocked);
}

QPointF GCursorControl::captureDeltaMovement(MovementMode mode) noexcept
{
	return _backend ? _backend->captureDeltaMovement(mode) : QPointF{};
}

bool GCursorControl::isLockSupported() const noexcept
{
	return lockState() != LockState::Unsupported;
}

void GCursorControl::updateLockState(LockState state)
{
	const auto previous = _lockState.exchange(state, std::memory_order_acq_rel);
	if (previous != state) { Q_EMIT lockStateChanged(state); }
}
}  // namespace gentau
