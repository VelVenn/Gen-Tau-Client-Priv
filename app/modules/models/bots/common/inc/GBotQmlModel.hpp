#pragma once

#include "bots/common/GBotModel.hpp"

#include <QtQmlIntegration/qqmlintegration.h>

namespace gentau {
class GBotQmlModel
{
	Q_GADGET

	QML_FOREIGN(GBotModel)
	QML_NAMED_ELEMENT(BotModel)
	QML_UNCREATABLE("BotModel is managed internally")
};
}  // namespace gentau