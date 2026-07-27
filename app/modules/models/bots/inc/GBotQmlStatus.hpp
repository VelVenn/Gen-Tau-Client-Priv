#pragma once

#include "bots/GBotStatus.hpp"

#include <QtQmlIntegration/qqmlintegration.h>

namespace gentau {
class GBotQmlStatus
{
	Q_GADGET

	QML_FOREIGN(GBotStatus)
	QML_NAMED_ELEMENT(BotStatus)
	QML_UNCREATABLE("BotStatus is owned by Context")
};
}  // namespace gentau