pragma Singleton

import QtQuick

QtObject {
    id: root

    enum BaseStatus {
        INVINCIBLE,
        ARMOR_CLOSED,
        ARMOR_OPENED
    }

    enum OutpostStatus {
        INVINCIBLE,
        ARMOR_SPINNING,
        ARMOR_IDLE,
        UNRECONSTRUCTABLE,
        RECONSTRUCTABLE,
        RECONSTRUCTING
    }
}
