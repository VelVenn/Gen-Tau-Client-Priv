#include "input/GInputEventDispatcher.hpp"

#include "adapter/mqtt/GMqttAdapter.hpp"
#include "input/GCursorControl.hpp"

#include <QThread>

#include <atomic>
#include <cmath>

#include "utils/TLog.hpp"

#define T_LOG_TAG "[Input] "

using namespace std;
using namespace std::chrono_literals;

namespace gentau {
optional<quint32> GInputEventDispatcher::keyMask(Qt::Key key) const noexcept
{
	switch (key) {
		case Qt::Key_W:
			return 1u << 0;
		case Qt::Key_S:
			return 1u << 1;
		case Qt::Key_A:
			return 1u << 2;
		case Qt::Key_D:
			return 1u << 3;
		case Qt::Key_Shift:
			return 1u << 4;
		case Qt::Key_Control:
			return 1u << 5;
		case Qt::Key_Q:
			return 1u << 6;
		case Qt::Key_E:
			return 1u << 7;
		case Qt::Key_R:
			return 1u << 8;
		case Qt::Key_F:
			return 1u << 9;
		case Qt::Key_G:
			return 1u << 10;
		case Qt::Key_Z:
			return 1u << 11;
		case Qt::Key_X:
			return 1u << 12;
		case Qt::Key_C:
			return 1u << 13;
		case Qt::Key_V:
			return 1u << 14;
		case Qt::Key_B:
			return 1u << 15;
		default:
			return nullopt;
	}
}

void GInputEventDispatcher::setInputBlocked(bool blocked)
{
	if (_inputBlocked == blocked) { return; }

	_inputBlocked = blocked;

	updateInputStatus();
}

KeyboardMouseControl GInputEventDispatcher::captureInput()
{
	KeyboardMouseControl msg;

	auto curInputStatus = _inputStatus.load();

	if (curInputStatus != InputStatus::Captured) {
		msg.setMouseX(0);
		msg.setMouseY(0);
		msg.setMouseZ(0);
		msg.setLeftButtonDown(false);
		msg.setRightButtonDown(false);
		msg.setMidButtonDown(false);
		msg.setKeyboardValue(0);

		return msg;
	}

	const auto [dX, dY] = _cursorControl->captureDeltaMovement();

	msg.setMouseX(static_cast<qint32>(round(dX)));
	msg.setMouseY(static_cast<qint32>(round(-dY)));
	msg.setMouseZ(_mouseZ.exchange(0, memory_order_relaxed));
	msg.setKeyboardValue(_keyboardValue.load(memory_order_relaxed));
	msg.setLeftButtonDown(_leftButtonDown.load(memory_order_relaxed));
	msg.setRightButtonDown(_rightButtonDown.load(memory_order_relaxed));
	msg.setMidButtonDown(_middleButtonDown.load(memory_order_relaxed));

	return msg;
}

void GInputEventDispatcher::resetInputState()
{
	_mouseZ.store(0, memory_order_relaxed);
	_leftButtonDown.store(false, memory_order_relaxed);
	_rightButtonDown.store(false, memory_order_relaxed);
	_middleButtonDown.store(false, memory_order_relaxed);
	_keyboardValue.store(0, memory_order_relaxed);

	if (_cursorControl) { _cursorControl->captureDeltaMovement(); }

	Q_EMIT newKeyboardEvent(KeyboardEventInfo{});
}

void GInputEventDispatcher::setInputStatus(InputStatus newStatus)
{
	auto oldStatus = _inputStatus.exchange(newStatus);

	if (oldStatus != newStatus) {
		tLogTrace(
			"Input status changed from {} to {}",
			static_cast<int>(oldStatus),
			static_cast<int>(newStatus)
		);

		resetInputState();

		Q_EMIT inputStatusChanged(newStatus);
	}
}

void GInputEventDispatcher::updateInputStatus()
{
	if (!_window) {
		setInputStatus(InputStatus::Unbound);
		return;
	}

	const bool shouldCapture = !_inputBlocked.load() && _window->isActive() &&
							   _window->isExposed() && _window->isVisible();

	InputStatus nextStatus = shouldCapture ? InputStatus::Captured : InputStatus::Suspended;

	setInputStatus(nextStatus);

	updateCursorState();
}

void GInputEventDispatcher::hideCursor()
{
	if (!_window || _savedCursor.has_value()) { return; }

	_savedCursor = _window->cursor();
	_window->setCursor(Qt::BlankCursor);
}

void GInputEventDispatcher::restoreCursor()
{
	if (!_savedCursor.has_value()) { return; }

	if (_window) { _window->setCursor(_savedCursor.value()); }

	_savedCursor.reset();
}

void GInputEventDispatcher::updateCursorState()
{
	if (!_cursorControl) { return; }

	const auto curStatus = _inputStatus.load();

	if (curStatus != InputStatus::Captured) {
		_cursorControl->unlock();
		restoreCursor();
	} else {
		switch (_cursorControl->lockState()) {
			case GCursorControl::LockState::Unsupported:
				hideCursor();
				break;
			case GCursorControl::LockState::Unlocked:
				hideCursor();
				_cursorControl->lock();
				break;
			default:
				break;
		}
	}
}

void GInputEventDispatcher::publishKeyboardMouseControl()
{
	auto msg = captureInput();

	auto payload = _serializer.serialize(&msg);

	if (_serializer.lastError() != QProtobufSerializer::Error::None) {
		// tLogError(
		// 	"Failed to serialize KeyboardMouseControl message: {}",
		// 	_serializer.lastErrorString().toStdString()
		// );

		return;
	}

	auto result = _client.publish(_curGen.load(), "KeyboardMouseControl", payload);

	// if (!result.succeeded()) {
	// 	tLogError("Failed to publish KeyboardMouseControl message: {}", result.cause.toStdString());
	// }
}

void GInputEventDispatcher::stopPubTask()
{
	if (!_taskHandle.has_value()) { return; }

	_scheduler.removeTask(_taskHandle.value());
	_taskHandle.reset();
}

void GInputEventDispatcher::startPubTask()
{
	if (_taskHandle.has_value()) { return; }

	_taskHandle = _scheduler.addTask(1.0s / 75, [this] { publishKeyboardMouseControl(); });
}

void GInputEventDispatcher::attachWindow(QQuickWindow* window)
{
	Q_ASSERT(QThread::currentThread() == thread());
	Q_ASSERT(window);
	Q_ASSERT(window->thread() == thread());

	if (!window) {
		tLogError("Invalid window provided to attach input event dispatcher");
		return;
	}

	if (_window == window) { return; }

	if (_window) {
		tLogWarn("Input event dispatcher is already attached, ignoring");
		return;
	}

	_window = window;
	_window->installEventFilter(this);

	_cursorControl = make_unique<GCursorControl>(window, this);

	connect(
		_cursorControl.get(),
		&GCursorControl::lockStateChanged,
		this,
		[this](GCursorControl::LockState state) { updateCursorState(); },
		Qt::QueuedConnection
	);

	connect(
		_window,
		&QQuickWindow::activeChanged,
		this,
		[this] { updateInputStatus(); },
		Qt::QueuedConnection
	);

	connect(
		_window,
		&QQuickWindow::visibleChanged,
		this,
		[this] { updateInputStatus(); },
		Qt::QueuedConnection
	);

	connect(window, &QObject::destroyed, this, [this] {
		setInputStatus(InputStatus::Unbound);
		restoreCursor();
	});

	updateInputStatus();
}

bool GInputEventDispatcher::eventFilter(QObject* watched, QEvent* event)
{
	if (!_window || watched != _window) { return QObject::eventFilter(watched, event); }

	switch (event->type()) {
		case QEvent::PlatformSurface: {
			auto* surfaceEvent = static_cast<QPlatformSurfaceEvent*>(event);

			// QQuickWindow 底层的原生 surface 可能会被多次销毁和创建，
			// 而 wayland 平台下的鼠标锁定依赖于原生 surface 的存在
			switch (surfaceEvent->surfaceEventType()) {
				case QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed:
					setInputStatus(InputStatus::Suspended);
					updateCursorState();
					break;

				case QPlatformSurfaceEvent::SurfaceCreated:
					updateInputStatus();
					break;
			}

			break;
		}
		case QEvent::ShortcutOverride: {
			auto* keyEvent = static_cast<QKeyEvent*>(event);

			if (keyEvent->key() == Qt::Key_P) { return false; }

			if (_inputStatus.load() == InputStatus::Captured) {
				keyEvent->accept();
				return true;
			} else {
				return false;
			}
		}
		case QEvent::Expose:
			updateInputStatus();
			break;
		case QEvent::KeyPress:
		case QEvent::KeyRelease:
			return handleKeyEvent(static_cast<QKeyEvent*>(event));
		case QEvent::Wheel:
			return handleMouseWheelEvent(static_cast<QWheelEvent*>(event));
		case QEvent::MouseButtonPress:
		case QEvent::MouseButtonDblClick:
			return handleMouseButtonEvent(static_cast<QMouseEvent*>(event), true);
		case QEvent::MouseButtonRelease:
			return handleMouseButtonEvent(static_cast<QMouseEvent*>(event), false);
		case QEvent::MouseMove:
			if (_inputStatus.load() == InputStatus::Captured) { return true; }
			return false;
		default:
			break;
	}

	return QObject::eventFilter(watched, event);
}

bool GInputEventDispatcher::handleKeyEvent(QKeyEvent* event)
{
	if (_inputStatus.load() != InputStatus::Captured) { return false; }

	if (event->isAutoRepeat()) { return true; }

	if (event->key() == Qt::Key_P) { return false; }

	auto mask = keyMask(static_cast<Qt::Key>(event->key()));

	if (mask.has_value()) {
		if (event->type() == QEvent::KeyPress) {
			_keyboardValue.fetch_or(mask.value(), memory_order_relaxed);
		} else if (event->type() == QEvent::KeyRelease) {
			_keyboardValue.fetch_and(~mask.value(), memory_order_relaxed);
		}

		return true;
	}

	Q_EMIT newKeyboardEvent(
		KeyboardEventInfo{ .key  = static_cast<Qt::Key>(event->key()),
						   .type = (event->type() == QEvent::KeyPress) ? KeyboardEventType::Press
																	   : KeyboardEventType::Release,
						   .modifiers = event->modifiers(),
						   .timestamp = event->timestamp() }
	);

	return true;
}

bool GInputEventDispatcher::handleMouseButtonEvent(QMouseEvent* event, bool pressed)
{
	if (_inputStatus.load() != InputStatus::Captured) { return false; }

	switch (event->button()) {
		case Qt::LeftButton:
			_leftButtonDown.store(pressed, memory_order_relaxed);
			return true;

		case Qt::RightButton:
			_rightButtonDown.store(pressed, memory_order_relaxed);
			return true;

		case Qt::MiddleButton:
			_middleButtonDown.store(pressed, memory_order_relaxed);
			return true;

		default:
			return false;
	}
}

bool GInputEventDispatcher::handleMouseWheelEvent(QWheelEvent* event)
{
	if (_inputStatus.load() != InputStatus::Captured) { return false; }

	_mouseZ.fetch_add(event->angleDelta().y(), std::memory_order_relaxed);

	return true;
}

GInputEventDispatcher::~GInputEventDispatcher()
{
	_scheduler.stop();

	if (_window) { _window->removeEventFilter(this); }

	if (_cursorControl) { _cursorControl->unlock(); }

	restoreCursor();
}

GInputEventDispatcher::GInputEventDispatcher(GMqttAdapter& client, QObject* parent) :
	QObject(parent),
	_client(client)
{
	qRegisterMetaType<KeyboardEventInfo>();

	_scheduler.run();

	connect(
		&_client,
		&GMqttAdapter::bindingChanged,
		this,
		[this](const QString& newId, const QString& newUri, quint64 newGen) {
			stopPubTask();
			_curGen.store(newGen);
		},
		Qt::QueuedConnection
	);

	connect(
		&_client, &GMqttAdapter::connected, this, [this] { startPubTask(); }, Qt::QueuedConnection
	);

	connect(
		&_client,
		&GMqttAdapter::connectionFailed,
		this,
		[this] { stopPubTask(); },
		Qt::QueuedConnection
	);

	connect(
		&_client,
		&GMqttAdapter::connectionLost,
		this,
		[this] { stopPubTask(); },
		Qt::QueuedConnection
	);
}
}  // namespace gentau