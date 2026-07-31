#pragma once

#include "bots/infantry/GInfantryModel.hpp"

#include <QtQmlIntegration/qqmlintegration.h>

namespace gentau {
class GInfantryQmlModel
{
	Q_GADGET

    QML_FOREIGN(GInfantryModel)
    QML_NAMED_ELEMENT(InfantryModel)
    QML_UNCREATABLE("InfantryModel is managed internally")
};
}  // namespace gentau