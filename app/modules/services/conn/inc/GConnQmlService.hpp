#pragma once

#include "services/conn/GConnService.hpp"

#include <QtQmlIntegration/qqmlintegration.h>

namespace gentau {
class GConnQmlService
{
	Q_GADGET

	QML_FOREIGN(GConnService)
	QML_NAMED_ELEMENT(ConnService)
	QML_UNCREATABLE("ConnService is owned by Context")
};
}  // namespace gentau