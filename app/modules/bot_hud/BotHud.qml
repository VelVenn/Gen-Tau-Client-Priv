import QtQuick

import Gentau.BotHud.Subpane

Item {
    id: root

    layer.enabled: true
    layer.samples: 4

    property real scaleFactor: 1.0

    implicitWidth: 1920
    implicitHeight: 1080

    HpHud {
        id: topHpHud

        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 2.5 * root.scaleFactor

        scaleFactor: root.scaleFactor

        // transformOrigin: Item.Bottom
    }

    ClientStatusPane {
        id: clientStatusHud

        anchors.left: topHpHud.right
        anchors.top: topHpHud.top

        anchors.topMargin: 8 * root.scaleFactor
        anchors.leftMargin: 100 * root.scaleFactor

        scaleFactor: root.scaleFactor
    }

    EcoHud {
        id: topEcoHud

        isOurRed: topHpHud.isOurRed

        anchors.top: topHpHud.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        // anchors.verticalCenter: topHpHud.verticalCenter
        // anchors.right: topHpHud.left

        anchors.topMargin: 5 * root.scaleFactor
        anchors.horizontalCenterOffset: -1 * root.scaleFactor

        scaleFactor: root.scaleFactor * 0.85

        // transformOrigin: Item.Top
    }

    TeamBotStatusPane {
        id: teamBotsStat

        anchors.horizontalCenter: topEcoHud.horizontalCenter
        anchors.top: topHpHud.bottom

        anchors.topMargin: 10 * root.scaleFactor

        scaleFactor: root.scaleFactor
    }

    CrosshairHud {
        id: centerCross

        anchors.centerIn: parent

        scaleFactor: root.scaleFactor
    }

    SpiltAmmoPane {
        id: centerAmmoHud

        anchors.centerIn: parent
        anchors.verticalCenterOffset: 2 * root.scaleFactor

        scaleFactor: root.scaleFactor
    }

    MyBotStatusHud {
        id: myBotStatus

        anchors.left: parent.left
        anchors.bottom: parent.bottom

        anchors.leftMargin: 50 * root.scaleFactor
        anchors.bottomMargin: 100 * root.scaleFactor

        scaleFactor: root.scaleFactor
    }

    MyBotModuleHud {
        id: myBotModule

        anchors.left: myBotStatus.left
        anchors.top: myBotStatus.bottom

        anchors.topMargin: 10 * root.scaleFactor

        scaleFactor: 1.1 * root.scaleFactor
    }

    HeroKeyHintPane {
        id: heroKeyHint

        anchors.horizontalCenter: centerCross.horizontalCenter
        anchors.bottom: myBotModule.bottom

        scaleFactor: root.scaleFactor
    }
}
