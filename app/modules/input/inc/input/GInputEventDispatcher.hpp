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
class GInputEventDispatcher : public QObject
{
	Q_OBJECT

  public:
	struct KeyboardMouseCtrlInfo  // Inside proto's KeyboardMouseControl message
	{
		qint32  mouseX{ 0 };
		qint32  mouseY{ 0 };
		qint32  mouseZ{ 0 };
		bool    leftButtonDown{ false };
		bool    rightButtonDown{ false };
		quint32 keyboardValue{ 0 };
		bool    middleButtonDown{ false };
	};

	struct KeyboardEventInfo  // Outside proto's KeyboardMouseControl message
	{};

	enum class InputStatus : quint32
	{
		Unbound = 0,
		Suspended,
		Captured
	};

  protected:
	bool eventFilter(QObject* watched, QEvent* event) override;

  private:
	void updateInputMode();

	std::optional<quint32> keyMask(Qt::Key key) const noexcept;

	KeyboardMouseControl captureInput();

  public:
	void attachWindow(QQuickWindow* window);

  public:
	bool uiBlocked() const noexcept { return _uiBlocked.load(); }
	void setUiBlocked(bool blocked) noexcept;

	InputStatus inputStatus() const noexcept { return _inputStatus.load(); }

  private:
	QPointer<QQuickWindow> _window;

	GMqttAdapter&       _client;
	QProtobufSerializer _serializer;
	TScheduler          _scheduler;

  private:
	std::atomic<qint32>  _mouseX{ 0 };
	std::atomic<qint32>  _mouseY{ 0 };
	std::atomic<qint32>  _mouseZ{ 0 };
	std::atomic<bool>    _leftButtonDown{ false };
	std::atomic<bool>    _rightButtonDown{ false };
	std::atomic<bool>    _middleButtonDown{ false };
	std::atomic<quint32> _keyboardValue{ 0 };

  private:
	QPointF _lastMouseGlobalPos{ 0.0, 0.0 };
	QPointF _anchorGlobalPos{ 0.0, 0.0 };

	std::atomic<bool>        _uiBlocked{ false };
	std::atomic<InputStatus> _inputStatus{ InputStatus::Unbound };

	std::atomic<quint64> _curGen{ 0 };

	// std::unique_ptr<GCursorControl> _cursorControl;
	// TODO: implement native wayland cursor capture and relative mouse movement recording

  public:
	explicit GInputEventDispatcher(GMqttAdapter& client, QObject* parent = nullptr);
	~GInputEventDispatcher() override;
};
}  // namespace gentau