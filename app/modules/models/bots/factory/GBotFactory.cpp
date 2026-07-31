#include "bots/factory/GBotFactory.hpp"

#include <QQuickItem>

#include "bots/common/GBotModel.hpp"
#include "bots/hero/GHeroModel.hpp"
#include "bots/infantry/GInfantryModel.hpp"

#include "utils/TLog.hpp"

#define T_LOG_TAG "[Bot Factory] "

using namespace std;

namespace gentau {
unique_ptr<GBotModel> GBotFactory::createBotModel(
	const QString& clientId, quint64 curGen, GBotCommonStatus& commonStatus
)
{
	bool    ok;
	quint32 idx = 0;

	idx = clientId.toUInt(&ok);

	if (!ok) {
		tLogWarn("Failed to convert clientId to usigned int: {}", clientId.toStdString());
		return nullptr;
	}

	tLogDebug("Recieved id: {}", idx);

	switch (idx) {
		case 1:
		case 101: {
			GHeroModel::InitPack initPack{ .client        = _client,
										   .curGen        = curGen,
										   .commonStatus  = commonStatus,
										   .deployVt      = _deployVt,
										   .inputCtrl     = _inputCtrl,
										   .imgTransQItem = _imgTransQItem,
										   .deployVtQItem = _deployVtQItem };

			return make_unique<GHeroModel>(std::move(initPack));
		}
		case 3:
		case 103:
		case 4:
		case 104: {
			GInfantryModel::InitPack initPack{ .client       = _client,
											   .curGen       = curGen,
											   .commonStatus = commonStatus };

			return make_unique<GInfantryModel>(std::move(initPack));
		}
		default:
			return nullptr;
	}
}

GBotFactory::GBotFactory(InitPack deps) :
	_imgTransQItem(deps.imgTransQItem),
	_deployVtQItem(deps.deployVtQItem),
	_deployVt(deps.deployVt),
	_client(deps.client),
	_inputCtrl(deps.inputCtrl)
{
	Q_ASSERT(_imgTransQItem);
	Q_ASSERT(_deployVtQItem);
}
}  // namespace gentau
