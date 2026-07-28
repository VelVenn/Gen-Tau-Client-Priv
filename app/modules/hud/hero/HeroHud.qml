import QtQuick

import Gentau.HeroHud.Subpane

import Gentau.Bot.Common
import Gentau.Bot.Hero

import Gentau.Message

import Gentau.Settings.UiPref

Item {
    id: root

    property real scaleFactor: 1.0

    required property HeroModel heroModel
    required property BotCommonStatus commonStatus

    readonly property robotStaticStatus staticStatus: root.commonStatus.staticStatus
    readonly property robotDynamicStatus dynamicStatus: root.commonStatus.dynamicStatus

    implicitWidth: 1920
    implicitHeight: 1080

    QtObject {
        id: param

        readonly property bool hintVisibleAllowed: 
            root.staticStatus.hasPerformanceSystemShooter && 
            root.staticStatus.performanceSystemShooter === BotCommonStatus.ShooterPerformance.HeroRanged
    }

    HeroKeyHintPane {
        id: hintPane

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 15 * root.scaleFactor

        heroModel: root.heroModel

        visible: param.hintVisibleAllowed
        alwaysShow: UiPref.showKeyHint

        scaleFactor: root.scaleFactor
    }
}
