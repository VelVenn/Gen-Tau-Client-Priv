#pragma once

#include "models/game_status/GGameStatusModel.hpp"

#include <QtQmlIntegration/qqmlintegration.h>

namespace gentau {
class GGameStatusQmlModel
{
	Q_GADGET

	QML_FOREIGN(GGameStatusModel)
    QML_NAMED_ELEMENT(GameStatusModel)
	QML_UNCREATABLE("GameStatus is owned by Context")
};
}  // namespace gentau
