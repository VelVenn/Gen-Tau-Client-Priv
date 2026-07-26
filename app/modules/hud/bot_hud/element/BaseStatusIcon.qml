import QtQuick

import Gentau.Foundation
import Gentau.CommonElem

HGradIconParal {
    id: root

    property int status: BaseMeta.BaseStatus.ARMOR_CLOSED

    baseColor: 'red'

    slantWidth: -8

    font.pixelSize: 16
    font.weight: 600

    borderWidth: 0

    verticalPadding: 0

    iconIndex: {
        switch (root.status) {
        case BaseMeta.BaseStatus.INVINCIBLE:
            return "\uecb3"; // Crown
        case BaseMeta.BaseStatus.ARMOR_CLOSED:
            return "\uf1cf"; // Close
        case BaseMeta.BaseStatus.ARMOR_OPENED:
            return "\uf1ce"; // Open
        default:
            return "\ueb8b"; // Question mark
        }
    }
}
