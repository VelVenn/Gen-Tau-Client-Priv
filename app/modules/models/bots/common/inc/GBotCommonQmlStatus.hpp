#pragma once

#include "bots/common/GBotCommonStatus.hpp"

#include <QtQmlIntegration/qqmlintegration.h>

namespace gentau {
class GBotCommonQmlStatus
{
	Q_GADGET

	QML_FOREIGN(GBotCommonStatus)
	QML_NAMED_ELEMENT(BotCommonStatus)
	QML_UNCREATABLE("BotCommonStatus is managed internally")
};
}  // namespace gentau
