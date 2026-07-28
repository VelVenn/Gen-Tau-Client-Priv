#include "bots/factory/GBotFactory.hpp"

#include <QQuickItem>

#include "bots/common/GBotModel.hpp"
#include "bots/hero/GHeroModel.hpp"

using namespace std;

namespace gentau {
unique_ptr<GBotModel> GBotFactory::createBotModel(
	const QString& clientId, quint64 curGen, GBotCommonStatus& commonStatus
)
{
	bool    ok;
	quint32 idx = 0;

	idx = clientId.toUInt(&ok);

	if (!ok) { return nullptr; }

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
