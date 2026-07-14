import QtQuick

import Gentau.Foundation
import Gentau.BasicWidgets
import Gentau.BotHud.Element

Item {
    id: root

    property bool isOurRed: true

    property int ourEco: 0
    // property int theirEco: 100

    property real ourFortOccupiedSec: 0.0
    property real theirFortOccupiedSec: 0.0

    property real maxFortOccupiedSec: 20.0

    property color ourColor: isOurRed ? Style.redColor : Style.blueColor
    property color theirColor: !isOurRed ? Style.redColor : Style.blueColor

    property real scaleFactor: 1.0

    implicitWidth: content.childrenRect.width * scaleFactor
    implicitHeight: content.childrenRect.height * scaleFactor

    layer.enabled: true
    layer.samples: 4

    Item {
        id: content

        transformOrigin: Item.TopLeft
        scale: root.scaleFactor
        x: -content.childrenRect.x * content.scale
        y: -content.childrenRect.y * content.scale

        Item {
            id: pivot

            width: 0
            height: 0
            x: 0
            y: 0
        }

        EcoValBadge {
            id: ourEcoBadge

            anchors.centerIn: pivot

            ecoVal: root.ourEco
            baseColor: root.ourColor

            z: 10
        }

        FortCountdownTimer {
            id: ourFortTimer

            isRed: root.isOurRed

            anchors.top: ourEcoBadge.top
            anchors.right: ourEcoBadge.left

            anchors.topMargin: 0
            anchors.rightMargin: 20

            value: root.ourFortOccupiedSec
            maxValue: root.maxFortOccupiedSec
        }

        FortCountdownTimer {
            id: theirFortTimer

            isRed: !root.isOurRed

            anchors.top: ourEcoBadge.top
            anchors.left: ourEcoBadge.right

            anchors.topMargin: 0
            anchors.leftMargin: 20

            value: root.theirFortOccupiedSec
            maxValue: root.maxFortOccupiedSec
        }
    }
}
