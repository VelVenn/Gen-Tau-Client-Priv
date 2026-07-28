#include "bots/GBotStatus.hpp"

#include "bots/common/GBotModel.hpp"
#include "bots/factory/GBotFactory.hpp"

using namespace std;

namespace gentau {
void GBotStatus::rebuildBotModel()
{
	if (!_curBingding.has_value() || !_factory) { return; }

	_botModel = _factory->createBotModel(_curBingding->clientId, _curBingding->gen, _commonStatus);

	Q_EMIT botModelChanged(_botModel.get());
}

void GBotStatus::setFactory(unique_ptr<GBotFactory> factory)
{
	Q_ASSERT(factory);
	Q_ASSERT(!_factory);

	_factory = std::move(factory);
	rebuildBotModel();
}

void GBotStatus::onBindingChanged(const QString& clientId, const QString&, quint64 gen)
{
	_curBingding = BindingInfo{ clientId, gen };
	rebuildBotModel();
}

GBotStatus::GBotStatus(GMqttAdapter& client, QObject* parent) :
	QObject(parent),
	_client(client),
	_commonStatus(_serializer, _client)
{
	connect(&_client, &GMqttAdapter::bindingChanged, this, &GBotStatus::onBindingChanged);
}

GBotStatus::~GBotStatus() = default;
}  // namespace gentau