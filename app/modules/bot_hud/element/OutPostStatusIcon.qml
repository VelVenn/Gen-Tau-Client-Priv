import QtQuick

import Gentau.Foundation
import Gentau.CommonElem

HGradIconParal {
    id: root

    property int status: BaseMeta.OutpostStatus.ARMOR_SPINNING


    QtObject {
        id: param
        property color displayIconColor
    }

    baseColor: 'red'

    slantWidth: -8

    font.pixelSize: 15
    font.weight: 600

    verticalPadding: 0

    iconIndex: {
        switch (root.status) {
        case BaseMeta.OutpostStatus.INVINCIBLE:
            return '\uecb3'; // Crown
        case BaseMeta.OutpostStatus.ARMOR_SPINNING:
            return '\ue863'; // Auto renew
        case BaseMeta.OutpostStatus.ARMOR_IDLE:
            return '\ue628'; // Sync disabled
        case BaseMeta.OutpostStatus.UNRECONSTRUCTABLE:
            return '\ue99a'; // Dangerous
        case BaseMeta.OutpostStatus.RECONSTRUCTABLE:
            return '\uea3c'; // Construction
        case BaseMeta.OutpostStatus.RECONSTRUCTING:
            return '\uea3c'; // Construction
        default:
            return '\ueb8b'; // Question mark
        }
    }

    PropertyAnimation {
        id: spinAnime
        running: root.status === BaseMeta.OutpostStatus.ARMOR_SPINNING && root.visible

        target: root
        property: "iconRotation"

        from: 0; to: 360

        duration: 2500

        easing.type: Easing.Linear
        loops: Animation.Infinite
    }

    SequentialAnimation {
        id: reconAnime
        running: root.status === BaseMeta.OutpostStatus.RECONSTRUCTING && root.visible
        loops: Animation.Infinite

        NumberAnimation {
            target: root
            property: "iconScale"
            from: 1.0; to: 1.3
            duration: 400
            easing.type: Easing.InOutQuad
        }

        NumberAnimation {
            target: root
            property: "iconScale"
            from: 1.3; to: 1.0
            duration: 600
            easing.type: Easing.OutBounce // 弹性回缩更有张力
        }

        onStopped: root.iconScale = 1.0
    }
}
