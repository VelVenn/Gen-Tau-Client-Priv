#pragma once

#include <QObject>
#include <QProtobufSerializer>

#include "adapter/mqtt/GMqttAdapter.hpp"

namespace gentau {
class GBaseModel : public QObject
{
	Q_OBJECT

	Q_PROPERTY(bool online READ online NOTIFY onlineChanged FINAL)

  Q_SIGNALS:
	void onlineChanged(bool online);

  public:
	bool online() const noexcept { return _online; }

  protected:
	QProtobufSerializer& serializer() noexcept { return _serializer; }
	GMqttAdapter&        client() noexcept { return _client; }

	virtual void resetStatus() {}

	virtual void onBindingChanged(const QString& newId, const QString& newUri, quint64 newGen) {}
	virtual void onConnected() {}
	virtual void onConnectionLost(const QString& cause) {}
	virtual void onConnectionFailed(const QString& cause) {}

  private:
	void updateOnline(bool online);

	void handleBindingChanged(const QString& newId, const QString& newUri, quint64 newGen);
	void handleConnected();
	void handleConnectionLost(const QString& cause);
	void handleConnectionFailed(const QString& cause);

  private:
	QProtobufSerializer& _serializer;
	GMqttAdapter&        _client;

  private:
	bool _online{ false };

  public:
	explicit GBaseModel(
		QProtobufSerializer& serializer, GMqttAdapter& client, QObject* parent = nullptr
	);
};
}  // namespace gentau
