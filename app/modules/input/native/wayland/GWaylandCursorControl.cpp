#include "native/wayland/GWaylandCursorControl.hpp"

#include <qguiapplication_platform.h>
#include <QGuiApplication>
#include <QThread>
#include <QWindow>

namespace gentau {
namespace {
constexpr double waylandFixedScale = 256.0;
constexpr int    protocolVersion   = 1;
}  // namespace

GWaylandRelativePointerManager::GWaylandRelativePointerManager() :
	QWaylandClientExtensionTemplate(protocolVersion)
{
	QObject::connect(this, &QWaylandClientExtension::activeChanged, this, [this] {
		if (!isActive() && QtWayland::zwp_relative_pointer_manager_v1::isInitialized()) {
			QtWayland::zwp_relative_pointer_manager_v1::destroy();
		}
	});
	initialize();
}

GWaylandRelativePointerManager::~GWaylandRelativePointerManager()
{
	if (QtWayland::zwp_relative_pointer_manager_v1::isInitialized()) {
		QtWayland::zwp_relative_pointer_manager_v1::destroy();
	}
}

GWaylandPointerConstraints::GWaylandPointerConstraints() :
	QWaylandClientExtensionTemplate(protocolVersion)
{
	QObject::connect(this, &QWaylandClientExtension::activeChanged, this, [this] {
		if (!isActive() && QtWayland::zwp_pointer_constraints_v1::isInitialized()) {
			QtWayland::zwp_pointer_constraints_v1::destroy();
		}
	});
	initialize();
}

GWaylandPointerConstraints::~GWaylandPointerConstraints()
{
	if (QtWayland::zwp_pointer_constraints_v1::isInitialized()) {
		QtWayland::zwp_pointer_constraints_v1::destroy();
	}
}

GWaylandCursorControl::GWaylandCursorControl(
	QWindow* window, LockStateChangedCallback lockStateChanged
) :
	GCursorControlBackend(std::move(lockStateChanged)),
	_window(window)
{
	QObject::connect(
		&_relativePointerManager,
		&QWaylandClientExtension::activeChanged,
		&_relativePointerManager,
		[this] { handleProtocolAvailabilityChanged(); }
	);
	QObject::connect(
		&_pointerConstraints,
		&QWaylandClientExtension::activeChanged,
		&_pointerConstraints,
		[this] { handleProtocolAvailabilityChanged(); }
	);
}

GWaylandCursorControl::~GWaylandCursorControl()
{
	unlock();

	if (QtWayland::zwp_relative_pointer_v1::isInitialized()) {
		QtWayland::zwp_relative_pointer_v1::destroy();
	}
}

bool GWaylandCursorControl::lock()
{
	if (!_window || !isLockSupported()) { return false; }

	Q_ASSERT(QThread::currentThread() == _window->thread());

	auto* application = qobject_cast<QGuiApplication*>(QCoreApplication::instance());
	auto* native      = application
							? application->nativeInterface<QNativeInterface::QWaylandApplication>()
							: nullptr;

	if (!native) { return false; }

	auto* pointer = native->pointer();
	auto* surface = reinterpret_cast<wl_surface*>(_window->winId());

	const bool pointerChanged = pointer != _pointer;
	const bool surfaceChanged = surface != _surface;
	const bool hasLockObject  = QtWayland::zwp_locked_pointer_v1::isInitialized();

	if (hasLockObject &&
		(pointerChanged || surfaceChanged ||
		 (!_lockPending && !_recordRelativeMotion.load(std::memory_order_acquire)))) {
		_recordRelativeMotion.store(false, std::memory_order_release);
		clearDeltaMovement();
		_lockPending = false;
		QtWayland::zwp_locked_pointer_v1::destroy();
	}

	if (pointerChanged && QtWayland::zwp_relative_pointer_v1::isInitialized()) {
		QtWayland::zwp_relative_pointer_v1::destroy();
	}

	_pointer = pointer;
	_surface = surface;

	if (!_pointer || !_surface || !isLockSupported()) {
		if (hasLockObject) {
			restoreCursor();
			notifyLockStateChanged(GCursorControl::LockState::Unlocked);
		}
		return false;
	}

	initializeRelativePointer();
	if (!QtWayland::zwp_relative_pointer_v1::isInitialized()) {
		if (hasLockObject) {
			restoreCursor();
			notifyLockStateChanged(GCursorControl::LockState::Unlocked);
		}
		return false;
	}

	if (QtWayland::zwp_locked_pointer_v1::isInitialized()) { return false; }

	auto* lockedPointer = _pointerConstraints.lock_pointer(
		_surface, _pointer, nullptr, QtWayland::zwp_pointer_constraints_v1::lifetime_persistent
	);
	if (!lockedPointer) {
		if (hasLockObject) {
			restoreCursor();
			notifyLockStateChanged(GCursorControl::LockState::Unlocked);
		}
		return false;
	}

	QtWayland::zwp_locked_pointer_v1::init(lockedPointer);
	_lockPending = true;
	_recordRelativeMotion.store(false, std::memory_order_release);
	clearDeltaMovement();

	hideCursor();

	return true;
}

void GWaylandCursorControl::unlock()
{
	if (_window) { Q_ASSERT(QThread::currentThread() == _window->thread()); }

	_recordRelativeMotion.store(false, std::memory_order_release);
	clearDeltaMovement();
	_lockPending = false;

	if (QtWayland::zwp_locked_pointer_v1::isInitialized()) {
		QtWayland::zwp_locked_pointer_v1::destroy();
	}

	restoreCursor();
}

bool GWaylandCursorControl::isLockSupported() const noexcept
{
	return _window && _pointerConstraints.isActive() && _relativePointerManager.isActive();
}

QPointF GWaylandCursorControl::captureDeltaMovement(GCursorControl::MovementMode mode) noexcept
{
	const auto acceleratedX   = _acceleratedX.exchange(0, std::memory_order_relaxed);
	const auto acceleratedY   = _acceleratedY.exchange(0, std::memory_order_relaxed);
	const auto unacceleratedX = _unacceleratedX.exchange(0, std::memory_order_relaxed);
	const auto unacceleratedY = _unacceleratedY.exchange(0, std::memory_order_relaxed);

	if (mode == GCursorControl::MovementMode::Unaccelerated) {
		return { static_cast<qreal>(unacceleratedX / waylandFixedScale),
				 static_cast<qreal>(unacceleratedY / waylandFixedScale) };
	}

	return { static_cast<qreal>(acceleratedX / waylandFixedScale),
			 static_cast<qreal>(acceleratedY / waylandFixedScale) };
}

void GWaylandCursorControl::handleProtocolAvailabilityChanged()
{
	if (isLockSupported()) {
		notifyLockStateChanged(GCursorControl::LockState::Unlocked);
		return;
	}

	unlock();
	if (QtWayland::zwp_relative_pointer_v1::isInitialized()) {
		QtWayland::zwp_relative_pointer_v1::destroy();
	}
	_pointer = nullptr;
	_surface = nullptr;
	notifyLockStateChanged(GCursorControl::LockState::Unsupported);
}

void GWaylandCursorControl::initializeRelativePointer()
{
	if (!_pointer || !_relativePointerManager.isActive() ||
		QtWayland::zwp_relative_pointer_v1::isInitialized()) {
		return;
	}

	auto* relativePointer = _relativePointerManager.get_relative_pointer(_pointer);
	if (relativePointer) { QtWayland::zwp_relative_pointer_v1::init(relativePointer); }
}

void GWaylandCursorControl::clearDeltaMovement() noexcept
{
	_acceleratedX.store(0, std::memory_order_relaxed);
	_acceleratedY.store(0, std::memory_order_relaxed);
	_unacceleratedX.store(0, std::memory_order_relaxed);
	_unacceleratedY.store(0, std::memory_order_relaxed);
}

void GWaylandCursorControl::hideCursor()
{
	if (_cursorHidden || !_window) { return; }

	_savedCursor = _window->cursor();
	_window->setCursor(Qt::BlankCursor);
	_cursorHidden = true;
}

void GWaylandCursorControl::restoreCursor()
{
	if (!_cursorHidden) { return; }

	if (_window) { _window->setCursor(_savedCursor); }
	_cursorHidden = false;
}

void GWaylandCursorControl::zwp_locked_pointer_v1_locked()
{
	_lockPending = false;
	clearDeltaMovement();
	_recordRelativeMotion.store(true, std::memory_order_release);
	hideCursor();
	notifyLockStateChanged(GCursorControl::LockState::Locked);
}

void GWaylandCursorControl::zwp_locked_pointer_v1_unlocked()
{
	_recordRelativeMotion.store(false, std::memory_order_release);
	clearDeltaMovement();
	_lockPending = false;
	restoreCursor();
	notifyLockStateChanged(GCursorControl::LockState::Unlocked);
}

void GWaylandCursorControl::zwp_relative_pointer_v1_relative_motion(
	uint32_t,
	uint32_t,
	wl_fixed_t dx,
	wl_fixed_t dy,
	wl_fixed_t dxUnaccelerated,
	wl_fixed_t dyUnaccelerated
)
{
	if (!_recordRelativeMotion.load(std::memory_order_acquire)) { return; }

	_acceleratedX.fetch_add(dx, std::memory_order_relaxed);
	_acceleratedY.fetch_add(dy, std::memory_order_relaxed);
	_unacceleratedX.fetch_add(dxUnaccelerated, std::memory_order_relaxed);
	_unacceleratedY.fetch_add(dyUnaccelerated, std::memory_order_relaxed);
}
}  // namespace gentau
