#pragma once

#include <QObject>
#include <QProtobufSerializer>

#include "adapter/mqtt/GMqttAdapter.hpp"

#include "bots/common/GBotCommonStatus.hpp"

Q_MOC_INCLUDE("bots/common/GBotModel.hpp")

#include <memory>

namespace gentau {
class GBotModel;

class GBotStatus : public QObject
{
	Q_OBJECT

	Q_PROPERTY(gentau::GBotCommonStatus* commonStatus READ commonStatus CONSTANT FINAL)
	Q_PROPERTY(gentau::GBotModel* botModel READ botModel NOTIFY botModelChanged FINAL)

  Q_SIGNALS:
	void botModelChanged(gentau::GBotModel* newBotModel);

  public:
	GBotCommonStatus* commonStatus() noexcept { return &_commonStatus; }
	GBotModel*        botModel() noexcept { return _botModel.get(); }

  private:
	QProtobufSerializer _serializer;
	GMqttAdapter&       _client;

  private:
	GBotCommonStatus _commonStatus;

	std::unique_ptr<GBotModel> _botModel{ nullptr };

  public:
	explicit GBotStatus(GMqttAdapter& client, QObject* parent = nullptr);

	~GBotStatus() override;
};
}  // namespace gentau