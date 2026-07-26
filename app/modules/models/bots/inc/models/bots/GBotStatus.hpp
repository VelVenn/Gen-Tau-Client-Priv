#pragma once

#include <QObject>
#include <QProtobufSerializer>

#include "adapter/mqtt/GMqttAdapter.hpp"

#include "bots/common/GBotCommonStatus.hpp"

namespace gentau {
class GBotStatus : public QObject
{
	Q_OBJECT

	Q_PROPERTY(gentau::GBotCommonStatus* commonStatus READ commonStatus CONSTANT FINAL)

  public:
	GBotCommonStatus* commonStatus() noexcept { return &_commonStatus; }

  private:
	QProtobufSerializer _serializer;
	GMqttAdapter&       _client;

  private:
	GBotCommonStatus _commonStatus;

  public:
	explicit GBotStatus(GMqttAdapter& client, QObject* parent = nullptr);

	~GBotStatus() override = default;
};
}  // namespace gentau