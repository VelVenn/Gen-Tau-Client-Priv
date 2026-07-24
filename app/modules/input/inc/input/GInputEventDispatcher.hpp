#pragma once

#include <QCursor>
#include <QEvent>
#include <QObject>
#include <QPointF>
#include <QProtobufSerializer>
#include <QQuickWindow>

#include "adapter/mqtt/GMqttAdapter.hpp"

#include "utils/TScheduler.hpp"

#include "message.qpb.h"

#include <atomic>
#include <optional>

namespace gentau {
class GCursorControl;

class GInputEventDispatcher : public QObject
{
	Q_OBJECT

  public:
	enum class KeyboardEventType : quint32
	{
		Press = 0,
		Release,
		Reset
	};
	Q_ENUM(KeyboardEventType)

	struct KeyboardEventInfo  // Outside proto's KeyboardMouseControl message
	{
		Qt::Key               key{ Qt::Key_unknown };
		KeyboardEventType     type{ KeyboardEventType::Reset };
		Qt::KeyboardModifiers modifiers{ Qt::NoModifier };
		quint64               timestamp{ 0 };
	};

	enum class InputStatus : quint32
	{
		Unbound = 0,
		Suspended,
		Captured
	};
	Q_ENUM(InputStatus)

	using InputBlockedId = quint64;

  Q_SIGNALS:
	void newKeyboardEvent(const KeyboardEventInfo& event);
	void inputStatusChanged(InputStatus newStatus);

  protected:
	bool eventFilter(QObject* watched, QEvent* event) override;

  private:
	bool handleKeyEvent(QKeyEvent* event);
	bool handleMouseWheelEvent(QWheelEvent* event);
	bool handleMouseButtonEvent(QMouseEvent* event, bool pressed);

  private:
	void updateInputStatus();

	void resetInputState();

	void setInputStatus(InputStatus newStatus);

	std::optional<quint32> keyMask(Qt::Key key) const noexcept;

	KeyboardMouseControl captureInput();

  private:
	void hideCursor();
	void restoreCursor();

	void updateCursorState();

	void publishKeyboardMouseControl();
	void startPubTask();
	void stopPubTask();

  public:
	void attachWindow(QQuickWindow* window);

  public:
	void setInputBlocked(bool blocked);

	void requestInputBlock(QObject* owner);
	void releaseInputBlock(QObject* owner);

	InputStatus inputStatus() const noexcept { return _inputStatus.load(); }

  private:
	std::atomic<qint32>  _mouseZ{ 0 };
	std::atomic<bool>    _leftButtonDown{ false };
	std::atomic<bool>    _rightButtonDown{ false };
	std::atomic<bool>    _middleButtonDown{ false };
	std::atomic<quint32> _keyboardValue{ 0 };

  private:
	QHash<QObject*, QMetaObject::Connection> _inputBlockRequests;

	std::atomic<bool>        _inputBlocked{ false };
	std::atomic<InputStatus> _inputStatus{ InputStatus::Unbound };

	std::optional<QCursor> _savedCursor{ std::nullopt };

	std::atomic<quint64>                  _curGen{ 0 };
	std::optional<TScheduler::TaskHandle> _taskHandle{ std::nullopt };

  private:
	QPointer<QQuickWindow>          _window;
	std::unique_ptr<GCursorControl> _cursorControl;

	GMqttAdapter&       _client;
	QProtobufSerializer _serializer;
	TScheduler          _scheduler;

  public:
	explicit GInputEventDispatcher(GMqttAdapter& client, QObject* parent = nullptr);
	~GInputEventDispatcher() override;
};
}  // namespace gentau

Q_DECLARE_METATYPE(gentau::GInputEventDispatcher::KeyboardEventInfo)
