#include "input/GInputEventDispatcher.hpp"
#include <qnamespace.h>
#include <qtprotobuftypes.h>
#include "input/GCursorControl.hpp"

#include <QThread>

#include <atomic>
#include <cmath>

#include "utils/TLog.hpp"

#define T_LOG_TAG "[Input] "

using namespace std;

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

void GInputEventDispatcher::setInputBlocked(bool blocked) noexcept
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
	auto oldStatus = _inputStatus.exchange(newStatus, memory_order_relaxed);

	if (oldStatus != newStatus) {
		tLogTrace(
			"Input status changed from {} to {}",
			static_cast<int>(oldStatus),
			static_cast<int>(newStatus)
		);

		Q_EMIT inputStatusChanged(newStatus);
	}
}

void GInputEventDispatcher::updateInputStatus()
{
	if (!_window) {
		setInputStatus(InputStatus::Unbound);
		return;
	}

	const bool shouldCapture = !_inputBlocked.load(std::memory_order_acquire) &&
							   _window->isActive() && _window->isExposed();

	if (!shouldCapture) {
		if (_cursorControl) { _cursorControl->unlock(); }

		// restore cursor
	} else {
		// hide cursor and try to lock it

		if (_cursorControl && _cursorControl->lockState() == GCursorControl::LockState::Unlocked) {
			_cursorControl->lock();
		}
	}
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

	// unfinished yet
}

GInputEventDispatcher::GInputEventDispatcher(GMqttAdapter& client, QObject* parent) :
	QObject(parent),
	_client(client)
{
	qRegisterMetaType<KeyboardEventInfo>();
}
}  // namespace gentau