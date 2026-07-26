import QtQuick

import Gentau.BotHud.Subpane

import Gentau.HeroHud

import Gentau.Settings.UiPref

import Gentau.Context

import Gentau.Message

Item {
    id: root

    layer.enabled: true
    layer.samples: 4

    readonly property int ourBotIdx: BotMeta.idStrToBotIdx(Context.connService.requestedId)

    readonly property int ourCamp: BotMeta.toBotCamp(root.ourBotIdx)

    readonly property bool hasKnownCamp: root.ourCamp !== BotMeta.BotCamp.UNKNOWN

    readonly property int displayOurCamp: root.hasKnownCamp ? root.ourCamp : BotMeta.BotCamp.RED

    readonly property bool isOurRed: root.displayOurCamp === BotMeta.BotCamp.RED

    readonly property robotStaticStatus botStaticStatus: Context.botStatus.commonStatus.staticStatus
    readonly property robotDynamicStatus botDynoStatus: Context.botStatus.commonStatus.dynamicStatus

    property real scaleFactor: 1.0

    implicitWidth: 1920
    implicitHeight: 1080

    HpHud {
        id: topHpHud

        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 2.5 * root.scaleFactor

        scaleFactor: root.scaleFactor

        globalStatus: Context.hudModel.globalStatus
        gameStatusModel: Context.hudModel.gameStatus

        isOurRed: root.isOurRed

        // transformOrigin: Item.Bottom
    }

    ClientStatusPane {
        id: clientStatusHud

        anchors.left: topHpHud.right
        anchors.top: topHpHud.top

        anchors.topMargin: 8 * root.scaleFactor
        anchors.leftMargin: 100 * root.scaleFactor

        connService: Context.connService

        scaleFactor: root.scaleFactor
    }

    EcoHud {
        id: topEcoHud

        globalStatus: Context.hudModel.globalStatus

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

        globalStatus: Context.hudModel.globalStatus

        ourCamp: root.displayOurCamp

        anchors.horizontalCenter: topEcoHud.horizontalCenter
        anchors.top: topHpHud.bottom

        anchors.topMargin: 10 * root.scaleFactor

        scaleFactor: root.scaleFactor
    }

    CrosshairHud {
        id: centerCross

        anchors.centerIn: parent

        scaleFactor: root.scaleFactor

        heatVal: root.botDynoStatus.currentHeat
        maxHeat: root.botStaticStatus.maxHeat

        visible: UiPref.showCrosshair
    }

    SpiltAmmoPane {
        id: centerAmmoHud

        anchors.centerIn: parent
        anchors.verticalCenterOffset: 2 * root.scaleFactor

        scaleFactor: root.scaleFactor

        leftAmmo: root.botDynoStatus.remainingAmmo
        ammoSpd: root.botDynoStatus.lastProjectileFireRate

        visible: UiPref.showSpdAndAmmo
    }

    MyBotStatusHud {
        id: myBotStatus

        anchors.left: parent.left
        anchors.bottom: parent.bottom

        anchors.leftMargin: 50 * root.scaleFactor
        anchors.bottomMargin: 100 * root.scaleFactor

        commonStatus: Context.botStatus.commonStatus

        scaleFactor: root.scaleFactor
    }

    MyBotModuleHud {
        id: myBotModule

        anchors.left: myBotStatus.left
        anchors.top: myBotStatus.bottom

        anchors.topMargin: 10 * root.scaleFactor

        commonStatus: Context.botStatus.commonStatus

        scaleFactor: 1.1 * root.scaleFactor
    }

    HeroHud {
        id: heroHud

        anchors.fill: parent

        scaleFactor: root.scaleFactor
    }
}
