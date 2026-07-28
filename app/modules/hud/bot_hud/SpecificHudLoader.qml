pragma ComponentBehavior: Bound

import QtQuick

import Gentau.Bot.Common
import Gentau.Bot.Hero

import Gentau.HeroHud

Item {
    id: root

    property real scaleFactor: 1.0

    required property BotModel botModel
    required property BotCommonStatus commonStatus

    readonly property HeroModel heroModel: botModel as HeroModel

    // onBotModelChanged: {
    //     console.log("[SpecificHud] botModel changed:", root.botModel, "heroModel:", root.heroModel);
    // }

    // onHeroModelChanged: {
    //     console.log("[SpecificHud] heroModel changed:", root.heroModel);
    // }

    // Component.onCompleted: {
    //     console.log("[SpecificHud] completed:", "botModel =", root.botModel, "heroModel =", root.heroModel, "showKeyHint =", UiPref.showKeyHint);
    // }

    Loader {
        anchors.fill: parent

        sourceComponent: {
            if (root.heroModel !== null) {
                return heroHud;
            }

            return null;
        }
    }

    Component {
        id: heroHud

        HeroHud {
            heroModel: root.heroModel
            commonStatus: root.commonStatus

            anchors.fill: parent

            scaleFactor: root.scaleFactor
        }
    }
}
