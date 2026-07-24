#include "services/conn/GConnService.hpp"

#include <QScopedValueRollback>
#include <QThread>

#include "utils/TLog.hpp"

#include <cstring>

#define T_LOG_TAG "[Connection Service] "

using namespace std;

namespace gentau {
QString GConnService::connModeToString(ConnMode mode) const noexcept
{
	switch (mode) {
		case ConnMode::Remote:
			return QStringLiteral("Remote");
		case ConnMode::Local:
			return QStringLiteral("Local");
		default:
			return QStringLiteral("None");
	}
}

void GConnService::bind(QString clientId, ConnMode mode)
{
	Q_ASSERT(QThread::currentThread() == thread());

	if (clientId.isEmpty() || mode == ConnMode::None) {
		tLogWarn("Invalid clientId or connMode provided for binding, ignored");
		return;
	}

	if (_bindInProgress) {
		tLogWarn("Reentrant bind request, ignored");
		return;
	}

	QScopedValueRollback guard(_bindInProgress, true);

	if (_clientId == clientId && _connMode == mode) {
		tLogWarn("Provided clientId and connMode same as current binding, ignored");
		return;
	}

	GMqttAdapter::BindResult cliBindRes;
	int                      udpBindRes  = 0;
	int                      udpStartRes = 0;

	const auto clientUri = (mode == ConnMode::Remote) ? remoteClientUri : localClientUri;
	const auto udpHost   = (mode == ConnMode::Remote) ? remoteUdpHost : localUdpHost;

	cliBindRes = _client.bind(clientId, QString::fromLatin1(clientUri));
	if (cliBindRes.succeeded()) { _lastChoseConnMode = mode; }

	udpBindRes = _imgTrans.receiver->bindV4(udpPort, udpHost);

	if (!cliBindRes.succeeded()) {
		tLogError("Failed to bind MQTT client: {}", cliBindRes.failedReason.toStdString());
	} else if (cliBindRes.changed()) {
		// Refresh the deploy VT pipeline after mqtt client bound.
		_deployVt.flush();
		_deployVt.restart();

		_deployVtEpoch = chrono::steady_clock::now();
		setDeployVtOnline(false);
	}

	bool udpReady = false;
	if (udpBindRes != 0) {
		tLogError("Failed to bind UDP receiver: [{}] {}", udpBindRes, strerror(udpBindRes));
	} else {
		// Refresh the imgTrans render pipeline after udp receiver bound.
		_imgTrans.renderer->flush();
		_imgTrans.renderer->restart();

		_vt13Epoch = chrono::steady_clock::now();
		setVt13Online(false);

		udpStartRes = _imgTrans.receiver->start();

		if (udpStartRes != 0) {
			tLogError("Failed to start UDP receiver: [{}] {}", udpStartRes, strerror(udpStartRes));
		} else {
			udpReady = true;
		}
	}

	if (cliBindRes.succeeded() && udpReady) {
		tLogInfo(
			"Successfully bound MQTT client and UDP receiver for clientId: {}, connMode: {}",
			clientId.toStdString(),
			connModeToString(mode).toStdString()
		);
	}

	return;
}

void GConnService::setVt13Online(bool online)
{
	if (_vt13Online == online) { return; }

	_vt13Online = online;
	Q_EMIT vt13OnlineChanged(online);
}

void GConnService::setDeployVtOnline(bool online)
{
	if (_deployVtOnline == online) { return; }

	_deployVtOnline = online;
	Q_EMIT deployVtOnlineChanged(online);
}

void GConnService::updateStreamStatus()
{
	const auto now      = chrono::steady_clock::now();
	const auto unset    = vid::TimePoint::min();
	const auto deadline = now - streamTimeoutInv;

	const auto vt13LastPushed     = _imgTrans.renderer->getLastPushSuccessTime();
	const auto deployVtLastPushed = _deployVt.getLastPushSuccessTime();

	const auto vt13Online =
		vt13LastPushed != unset && vt13LastPushed >= _vt13Epoch && vt13LastPushed >= deadline;

	const auto deployVtOnline = deployVtLastPushed != unset &&
								deployVtLastPushed >= _deployVtEpoch &&
								deployVtLastPushed >= deadline;

	setVt13Online(vt13Online);
	setDeployVtOnline(deployVtOnline);
}

void GConnService::setConnectedState()
{
	const bool idChanged   = _clientId != _lastChoseClientId;
	const bool modeChanged = _connMode != _lastChoseConnMode;

	_clientId = _lastChoseClientId;
	_connMode = _lastChoseConnMode;

	if (idChanged) { Q_EMIT clientIdChanged(_clientId); }

	if (modeChanged) { Q_EMIT connModeChanged(_connMode); }
}

void GConnService::clearConnectedState()
{
	const bool idChanged   = !_clientId.isEmpty();
	const bool modeChanged = _connMode != ConnMode::None;

	_clientId.clear();
	_connMode = ConnMode::None;

	if (idChanged) { Q_EMIT clientIdChanged(_clientId); }

	if (modeChanged) { Q_EMIT connModeChanged(_connMode); }
}

void GConnService::onBindingChanged(const QString& newId, const QString&, quint64)
{
	Q_ASSERT(QThread::currentThread() == thread());

	_lastChoseClientId = newId;

	clearConnectedState();
}

void GConnService::onConnected()
{
	Q_ASSERT(QThread::currentThread() == thread());

	tLogInfo("MQTT client connected");

	setConnectedState();
}

void GConnService::onConnectionLost(const QString& cause)
{
	Q_ASSERT(QThread::currentThread() == thread());

	tLogWarn("MQTT client connection lost: {}", cause.toStdString());

	clearConnectedState();
}

void GConnService::onConnectionFailed(const QString& cause)
{
	Q_ASSERT(QThread::currentThread() == thread());

	tLogWarn("MQTT client connection failed: {}", cause.toStdString());

	clearConnectedState();
}

GConnService::GConnService(
	TImgTrans& imgTrans, TBytesVidRender& deployVt, GMqttAdapter& client, QObject* parent
) :
	QObject(parent),
	_imgTrans(imgTrans),
	_deployVt(deployVt),
	_client(client)
{
	connect(&_client, &GMqttAdapter::bindingChanged, this, &GConnService::onBindingChanged);
	connect(&_client, &GMqttAdapter::connected, this, &GConnService::onConnected);
	connect(&_client, &GMqttAdapter::connectionLost, this, &GConnService::onConnectionLost);
	connect(&_client, &GMqttAdapter::connectionFailed, this, &GConnService::onConnectionFailed);

	connect(&_streamStatusTimer, &QTimer::timeout, this, &GConnService::updateStreamStatus);

	_streamStatusTimer.setInterval(streamStatusCheckInv);
	_streamStatusTimer.start();
}
}  // namespace gentau