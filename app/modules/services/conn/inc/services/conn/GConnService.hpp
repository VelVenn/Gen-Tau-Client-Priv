#pragma once

#include <QObject>
#include <QTimer>

#include "adapter/mqtt/GMqttAdapter.hpp"

#include "img_trans/TImgTrans.hpp"
#include "img_trans/vid_render/TBytesVidRender.hpp"
#include "img_trans/vid_render/TVidUtils.hpp"

#include <chrono>
#include <optional>

namespace gentau {
class GConnService : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString clientId READ clientId NOTIFY clientIdChanged)
	Q_PROPERTY(QString requestedId READ requestedId NOTIFY requestedIdChanged)
	Q_PROPERTY(ConnMode connMode READ connMode NOTIFY connModeChanged)

	Q_PROPERTY(bool vt13Online READ vt13Online NOTIFY vt13OnlineChanged)
	Q_PROPERTY(bool deployVtOnline READ deployVtOnline NOTIFY deployVtOnlineChanged)

  public:
	enum class ConnMode : quint8
	{
		Remote = 0,
		Local,
		None
	};
	Q_ENUM(ConnMode)

	enum class UdpBindResultType : quint8
	{
		Unchanged = 0,
		Failed,
		Changed
	};

	struct UdpConnectionResult
	{
		UdpBindResultType  bindResult{ UdpBindResultType::Unchanged };
		std::optional<int> bindErrorCode{ std::nullopt };
		std::optional<int> startResult{ std::nullopt };

		bool bindChanged() const noexcept { return bindResult == UdpBindResultType::Changed; }
		bool bindFailed() const noexcept { return bindResult == UdpBindResultType::Failed; }

		bool startAttempted() const noexcept { return startResult.has_value(); }
		bool startSucceeded() const noexcept
		{
			return startResult.has_value() && startResult.value() == 0;
		}
	};

  private:
	static constexpr auto remoteClientUri = "mqtt://192.168.12.2:3333";
	static constexpr auto localClientUri  = "mqtt://localhost:3333";

	static constexpr auto remoteUdpHost = "192.168.12.2";
	static constexpr auto localUdpHost  = "127.0.0.1";
	static constexpr auto udpPort       = 3334;

	static constexpr auto streamStatusCheckInv = 200;  // ms
	static constexpr auto streamTimeoutInv     = std::chrono::seconds(1);

  Q_SIGNALS:
	void clientIdChanged(QString newId);
	void requestedIdChanged(QString newId);
	void connModeChanged(ConnMode newMode);

	void vt13OnlineChanged(bool online);
	void deployVtOnlineChanged(bool online);

  public:
	Q_INVOKABLE void bind(QString clientId, ConnMode mode = ConnMode::Remote);

	Q_INVOKABLE QString connModeToString(ConnMode mode) const noexcept;

  public:
	QString  clientId() const noexcept { return _clientId; }
	QString  requestedId() const noexcept { return _requestedClientId; }
	ConnMode connMode() const noexcept { return _connMode; }

	bool vt13Online() const noexcept { return _vt13Online; }
	bool deployVtOnline() const noexcept { return _deployVtOnline; }

  private:
	void onBindingChanged(const QString& newId, const QString& newUri, quint64 newGen);
	void onConnected();
	void onConnectionLost(const QString& cause);
	void onConnectionFailed(const QString& cause);

	void setConnectedState();
	void clearConnectedState();

	void setRequestedClientId(const QString& id);

	void updateStreamStatus();

	void setVt13Online(bool online);
	void setDeployVtOnline(bool online);

	void refreshImgTransPipeline();
	void refreshDeployVtPipeline();

	GMqttAdapter::BindResult applyClientBinding(const QString& clientId, ConnMode mode);
	UdpConnectionResult      applyUdpBinding(ConnMode mode);

  private:
	QString _requestedClientId{ "" };
	QString _clientId{ "" };

	ConnMode _lastChoseConnMode{ ConnMode::None };
	ConnMode _connMode{ ConnMode::None };

	bool           _vt13Online{ false };
	bool           _deployVtOnline{ false };
	vid::TimePoint _vt13Epoch{ vid::TimePoint::min() };
	vid::TimePoint _deployVtEpoch{ vid::TimePoint::min() };

	bool _bindInProgress{ false };

  private:
	TImgTrans&       _imgTrans;
	TBytesVidRender& _deployVt;
	GMqttAdapter&    _client;

	QTimer _streamStatusTimer;

  public:
	GConnService(
		TImgTrans&       imgTrans,
		TBytesVidRender& deployVt,
		GMqttAdapter&    client,
		QObject*         parent = nullptr
	);
	~GConnService() override = default;
};
}  // namespace gentau