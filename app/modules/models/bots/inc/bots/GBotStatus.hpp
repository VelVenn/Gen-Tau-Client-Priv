#pragma once

#include <QObject>
#include <QProtobufSerializer>

#include "adapter/mqtt/GMqttAdapter.hpp"

#include "bots/common/GBotCommonStatus.hpp"

Q_MOC_INCLUDE("bots/common/GBotModel.hpp")

#include <memory>
#include <optional>

namespace gentau {
class GBotFactory;
class GBotModel;

class GBotStatus : public QObject
{
	Q_OBJECT

	Q_PROPERTY(gentau::GBotCommonStatus* commonStatus READ commonStatus CONSTANT FINAL)
	Q_PROPERTY(gentau::GBotModel* botModel READ botModel NOTIFY botModelChanged FINAL)

  Q_SIGNALS:
	void botModelChanged(gentau::GBotModel* newBotModel);

  public:
	struct BindingInfo
	{
		QString clientId;
		quint64 gen;
	};

  public:
	GBotCommonStatus* commonStatus() noexcept { return &_commonStatus; }
	GBotModel*        botModel() noexcept { return _botModel.get(); }

	void setFactory(std::unique_ptr<GBotFactory> factory);

  private:
	void onBindingChanged(const QString& clientId, const QString&, quint64 gen);
	void rebuildBotModel();

  private:
	QProtobufSerializer _serializer;
	GMqttAdapter&       _client;

  private:
	GBotCommonStatus _commonStatus;

	std::optional<BindingInfo>   _curBingding{ std::nullopt };
	std::unique_ptr<GBotFactory> _factory{ nullptr };
	std::unique_ptr<GBotModel>   _botModel{ nullptr };

  public:
	explicit GBotStatus(GMqttAdapter& client, QObject* parent = nullptr);

	~GBotStatus() override;
};
}  // namespace gentau