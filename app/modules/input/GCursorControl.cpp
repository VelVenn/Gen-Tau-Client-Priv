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
		_backend = std::make_unique<GWaylandCursorControl>(window);
	}
#else
	Q_UNUSED(window);
#endif
}

GCursorControl::~GCursorControl() = default;

void GCursorControl::lock()
{
	if (_backend) { _backend->lock(); }
}

void GCursorControl::unlock()
{
	if (_backend) { _backend->unlock(); }
}

QPointF GCursorControl::getDeltaMovement(MovementMode mode) noexcept
{
	return _backend ? _backend->getDeltaMovement(mode) : QPointF{};
}
}  // namespace gentau
