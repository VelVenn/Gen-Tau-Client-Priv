#pragma once

#include "models/hud/GHudModel.hpp"

#include <QtQmlIntegration/qqmlintegration.h>

namespace gentau {
class GHudQmlModel
{
	Q_GADGET

	QML_FOREIGN(GHudModel)
	QML_NAMED_ELEMENT(HudModel)
	QML_UNCREATABLE("HudModel is owned by Context")
};
}  // namespace gentau