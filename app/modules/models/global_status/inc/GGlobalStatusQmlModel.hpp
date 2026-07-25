#pragma once

#include "models/global_status/GGlobalStatusModel.hpp"

#include <QtQmlIntegration/qqmlintegration.h>

namespace gentau {
class GGlobalStatusQmlModel
{
    Q_GADGET

    QML_FOREIGN(GGlobalStatusModel)
    QML_NAMED_ELEMENT(GlobalStatus)
    QML_UNCREATABLE("GlobalStatus is owned by Context")
};
}  // namespace gentau