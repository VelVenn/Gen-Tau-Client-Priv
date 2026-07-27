#pragma once

#include "bots/hero/GHeroModel.hpp"

#include <QtQmlIntegration/qqmlintegration.h>

namespace gentau {
class GHeroQmlModel
{
	Q_GADGET

	QML_FOREIGN(GHeroModel)
	QML_NAMED_ELEMENT(HeroModel)
	QML_UNCREATABLE("HeroModel is managed internally")
};
}  // namespace gentau
