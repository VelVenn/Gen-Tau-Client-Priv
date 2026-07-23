#pragma once

#include <qnamespace.h>
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

  Q_SIGNALS:
	void newKeyboardEvent(const KeyboardEventInfo& event);
	void inputStatusChanged(InputStatus newStatus);

  protected:
	bool eventFilter(QObject* watched, QEvent* event) override;

  private:
	void updateInputStatus();

	void resetInputState();

	void setInputStatus(InputStatus newStatus);

	std::optional<quint32> keyMask(Qt::Key key) const noexcept;

	KeyboardMouseControl captureInput();

  public:
	void attachWindow(QQuickWindow* window);

  public:
	bool inputBlocked() const noexcept { return _inputBlocked.load(); }
	void setInputBlocked(bool blocked) noexcept;

	InputStatus inputStatus() const noexcept { return _inputStatus.load(); }

  private:
	QPointer<QQuickWindow>          _window;
	std::unique_ptr<GCursorControl> _cursorControl;

	GMqttAdapter&       _client;
	QProtobufSerializer _serializer;
	TScheduler          _scheduler;

  private:
	std::atomic<qint32>  _mouseZ{ 0 };
	std::atomic<bool>    _leftButtonDown{ false };
	std::atomic<bool>    _rightButtonDown{ false };
	std::atomic<bool>    _middleButtonDown{ false };
	std::atomic<quint32> _keyboardValue{ 0 };

  private:
	QPointF _lastMouseGlobalPos{ 0.0, 0.0 };
	QPointF _anchorGlobalPos{ 0.0, 0.0 };

	std::atomic<bool>        _inputBlocked{ false };
	std::atomic<InputStatus> _inputStatus{ InputStatus::Unbound };

	std::atomic<quint64> _curGen{ 0 };

  public:
	explicit GInputEventDispatcher(GMqttAdapter& client, QObject* parent = nullptr);
	~GInputEventDispatcher() override;
};
}  // namespace gentau

Q_DECLARE_METATYPE(gentau::GInputEventDispatcher::KeyboardEventInfo)
