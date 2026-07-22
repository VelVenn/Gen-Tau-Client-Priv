#include "input/GInputEventDispatcher.hpp"
#include <qnamespace.h>

#include <QThread>
#include <atomic>

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

void GInputEventDispatcher::setUiBlocked(bool blocked) noexcept
{
	if (_uiBlocked == blocked) { return; }

	_uiBlocked = blocked;

	// unfinished yet
	// updateInputMode();
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

	msg.setMouseX(_mouseX.exchange(0, memory_order_relaxed));
	msg.setMouseY(_mouseY.exchange(0, memory_order_relaxed));
	msg.setMouseZ(_mouseZ.exchange(0, memory_order_relaxed));
	msg.setKeyboardValue(_keyboardValue.load(memory_order_relaxed));
	msg.setLeftButtonDown(_leftButtonDown.load(memory_order_relaxed));
	msg.setRightButtonDown(_rightButtonDown.load(memory_order_relaxed));
	msg.setMidButtonDown(_middleButtonDown.load(memory_order_relaxed));

	return msg;
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

	// unfinished yet
}
}  // namespace gentau