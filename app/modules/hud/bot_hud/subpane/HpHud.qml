import QtQuick
import QtQuick.Shapes
import QtQuick.Window

import Gentau.Foundation
import Gentau.BasicWidgets
import Gentau.CommonElem
import Gentau.BotHud.Element

import Gentau.Model.GlobalStatus
import Gentau.Model.GameStatus
import Gentau.Message

Item {
    id: root

    property real scaleFactor: 1.0

    required property GlobalStatus globalStatus
    required property GameStatusModel gameStatusModel
    required property bool isOurRed

    readonly property globalUnitStatus unitStatus: globalStatus.unitStatus
    readonly property globalLogisticsStatus logisticsStatus: globalStatus.logisticsStatus
    readonly property gameStatus roundStatus: gameStatusModel.roundStatus

    // Our Data Properties
    readonly property real ourBaseHp: unitStatus.baseHealth
    readonly property real ourBaseMaxHp: 5000
    readonly property real ourBaseDef: unitStatus.baseShield
    readonly property int ourBaseStatus: {
        if (root.globalStatus.online) {
            return root.unitStatus.baseStatus;
        }

        return BaseMeta.BaseStatus.ARMOR_CLOSED;
    }

    readonly property real ourOutpostHp: unitStatus.outpostHealth
    readonly property real ourOutpostMaxHp: 1500
    readonly property real ourOutpostDef: 0
    readonly property int ourOutpostStatus: {
        if (root.globalStatus.online) {
            return root.unitStatus.outpostStatus;
        }

        return BaseMeta.OutpostStatus.ARMOR_IDLE;
    }

    readonly property int ourScore: {
        if (root.isOurRed) {
            return root.roundStatus.redScore;
        }

        return root.roundStatus.blueScore;
    }

    // Their Data Properties
    readonly property real theirBaseHp: unitStatus.enemyBaseHealth
    readonly property real theirBaseMaxHp: 5000
    readonly property real theirBaseDef: unitStatus.enemyBaseShield
    readonly property int theirBaseStatus: {
        if (root.globalStatus.online) {
            return root.unitStatus.enemyBaseStatus;
        }

        return BaseMeta.BaseStatus.ARMOR_CLOSED;
    }

    readonly property real theirOutpostHp: unitStatus.enemyOutpostHealth
    readonly property real theirOutpostMaxHp: 1500
    readonly property real theirOutpostDef: 0
    readonly property int theirOutpostStatus: {
        if (root.globalStatus.online) {
            return root.unitStatus.enemyOutpostStatus;
        }

        return BaseMeta.OutpostStatus.ARMOR_IDLE;
    }

    readonly property int theirScore: {
        if (root.isOurRed) {
            return root.roundStatus.blueScore;
        }

        return root.roundStatus.redScore;
    }

    readonly property string centralTimeText: {
        if (!root.gameStatusModel.online || !root.roundStatus.hasStageCountdownSec) {
            return '--:--';
        }

        const totalSeconds = Math.max(0, Math.floor(root.roundStatus.stageCountdownSec));

        const minutes = Math.floor(totalSeconds / 60);
        const seconds = totalSeconds % 60;
        const paddedSeconds = seconds < 10 ? '0' + seconds : seconds;

        return minutes + ':' + paddedSeconds;
    }

    property string gameCountText: {
        if (!root.gameStatusModel.online) {
            return '- / -';
        }

        const currentRound = root.roundStatus.hasCurrentRound ? root.roundStatus.currentRound : '-';

        const totalRounds = root.roundStatus.hasTotalRounds ? root.roundStatus.totalRounds : '-';

        return currentRound + ' / ' + totalRounds;
    }
    // property string phaseText: '三分钟准备'

    readonly property color redColor: Style.redColor
    readonly property color blueColor: Style.blueColor
    readonly property color lightBlue: Style.lightBlue
    readonly property color metallicGold: Style.metallicGold
    readonly property color grayColor: Style.grayColor

    implicitWidth: content.childrenRect.width * scaleFactor
    implicitHeight: content.childrenRect.height * scaleFactor

    layer.enabled: true
    layer.samples: 4

    antialiasing: true

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

        TrapeziodBadge {
            id: centralBadge

            anchors.horizontalCenter: pivot.horizontalCenter
            anchors.top: pivot.top

            text: root.centralTimeText
            textOffsetY: 1

            bottomIndent: 15

            font.family: Style.firaCodeFL.font.family
            font.pixelSize: 35
            textColor: 'black'

            width: 145

            borderColor: Qt.darker(Qt.alpha(root.lightBlue, 0.6), 1.2)

            borderWidth: 3
            borderRadius: 6

            badgeGradient: LinearGradient {
                x1: 0
                y1: 0
                x2: 0
                y2: centralBadge.height

                GradientStop {
                    position: 0.0
                    color: root.lightBlue
                }
                GradientStop {
                    position: 0.3
                    color: Qt.alpha(root.lightBlue, 0.6)
                }
                GradientStop {
                    position: 0.6
                    color: Qt.alpha(root.lightBlue, 0.4)
                }
                GradientStop {
                    position: 1.0
                    color: Qt.alpha(root.lightBlue, 0.2)
                }
            }
        }

        Text {
            id: ourScoreBadge

            anchors.verticalCenter: ourBaseHpBar.verticalCenter
            anchors.right: centralBadge.left

            anchors.verticalCenterOffset: 3
            anchors.rightMargin: 5

            font.family: Style.firaCodeFL.font.family
            font.pixelSize: 40
            font.bold: true

            color: root.isOurRed ? root.redColor : root.blueColor

            text: root.ourScore.toString()
        }

        Text {
            id: theirScoreBadge

            anchors.verticalCenter: theirBaseHpBar.verticalCenter
            anchors.left: centralBadge.right

            anchors.verticalCenterOffset: 3
            anchors.leftMargin: 5

            font.family: Style.firaCodeFL.font.family
            font.pixelSize: 40
            font.bold: true

            color: !root.isOurRed ? root.redColor : root.blueColor

            text: root.theirScore.toString()
        }

        RectBadge {
            id: gameCount

            anchors.horizontalCenter: pivot.horizontalCenter
            anchors.bottom: centralBadge.top
            anchors.bottomMargin: 5

            font.family: Style.firaCodeFL.font.family
            font.pixelSize: 12
            font.bold: true

            borderWidth: 2
            borderRadius: 4
            borderColor: Qt.darker(Qt.alpha(root.lightBlue, 0.6), 1.2)

            badgeGradient: Gradient {
                GradientStop {
                    position: 0.0
                    color: Qt.alpha(root.lightBlue, 0.2)
                }
                GradientStop {
                    position: 0.3
                    color: Qt.alpha(root.lightBlue, 0.4)
                }
                GradientStop {
                    position: 0.6
                    color: Qt.alpha(root.lightBlue, 0.6)
                }
                GradientStop {
                    position: 1.0
                    color: root.lightBlue
                }
            }

            text: root.gameCountText
            textColor: 'black'
            textOffsetY: 1
        }

        HpBar {
            id: ourBaseHpBar

            isInvincible: root.ourBaseStatus === BaseMeta.BaseStatus.INVINCIBLE

            targetHp: root.ourBaseHp
            baseColor: root.isOurRed ? root.redColor : root.blueColor
            invincibleColor: root.metallicGold

            bgColor: Qt.alpha(root.grayColor, 0.6)

            anchors.right: ourScoreBadge.left
            anchors.bottom: centralBadge.bottom
            anchors.bottomMargin: 10
            anchors.rightMargin: 10

            width: 400

            maxValue: root.ourBaseMaxHp
            slantWidth: -10
            barFromRight: true

            radiusBL: 5
            radiusTL: 5
        }

        HpBar {
            id: theirBaseHpBar

            isInvincible: root.theirBaseStatus === BaseMeta.BaseStatus.INVINCIBLE

            targetHp: root.theirBaseHp
            baseColor: !root.isOurRed ? root.redColor : root.blueColor
            invincibleColor: root.metallicGold

            bgColor: Qt.alpha(root.grayColor, 0.6)

            anchors.left: theirScoreBadge.right
            anchors.bottom: centralBadge.bottom
            anchors.bottomMargin: 10
            anchors.leftMargin: 10

            width: 400

            maxValue: root.theirBaseMaxHp
            slantWidth: 10
            textAlignment: Qt.AlignRight

            radiusBR: 5
            radiusTR: 5
        }

        BaseStatusIcon {
            id: ourBaseStatusIcon

            baseColor: root.isOurRed ? root.redColor : root.blueColor

            anchors.top: ourBaseHpBar.bottom
            anchors.left: ourBaseHpBar.left
            anchors.topMargin: 5
            anchors.leftMargin: 14

            status: root.ourBaseStatus

            slantWidth: -8
            gradientPreset: GradientPreset.LighterOnRight
        }

        BaseStatusIcon {
            id: theirBaseStatusIcon

            baseColor: !root.isOurRed ? root.redColor : root.blueColor

            anchors.top: theirBaseHpBar.bottom
            anchors.right: theirBaseHpBar.right
            anchors.topMargin: 5
            anchors.rightMargin: 14

            status: root.theirBaseStatus

            slantWidth: 8
            gradientPreset: GradientPreset.LighterOnLeft
        }

        DefIconBadge {
            id: ourBaseDefBadge

            defVal: root.ourBaseDef

            baseColor: root.isOurRed ? root.redColor : root.blueColor

            anchors.top: ourBaseHpBar.bottom
            anchors.left: ourBaseStatusIcon.right
            anchors.topMargin: 5
            anchors.leftMargin: -5

            slantWidth: -8
        }

        DefIconBadge {
            id: theirBaseDefBadge

            defVal: root.theirBaseDef

            baseColor: !root.isOurRed ? root.redColor : root.blueColor

            anchors.top: theirBaseHpBar.bottom
            anchors.right: theirBaseStatusIcon.left
            anchors.topMargin: 5
            anchors.rightMargin: -5

            slantWidth: 8

            lighterOnLeft: true
            iconOnLeft: false
        }

        HorizonGradParalBadge {
            id: ourOutpostBadge

            baseColor: root.isOurRed ? root.redColor : root.blueColor

            anchors.bottom: ourOutpostHpBar.top
            anchors.left: ourOutpostHpBar.left
            anchors.leftMargin: -10

            slantWidth: -10

            text: 'OUTPOST'
        }

        HorizonGradParalBadge {
            id: theirOutpostBadge

            baseColor: !root.isOurRed ? root.redColor : root.blueColor

            anchors.bottom: theirOutpostHpBar.top
            anchors.right: theirOutpostHpBar.right
            anchors.rightMargin: -13 // OutpostHpBar 的边框厚度导致的偏移

            slantWidth: 10

            lighterOnLeft: true

            text: 'OUTPOST'
        }

        HpBar {
            id: ourOutpostHpBar

            isInvincible: root.ourOutpostStatus === BaseMeta.OutpostStatus.INVINCIBLE

            targetHp: root.ourOutpostHp
            baseColor: root.isOurRed ? root.redColor : root.blueColor
            invincibleColor: root.metallicGold

            bgColor: Qt.alpha(root.grayColor, 0.6)

            anchors.verticalCenter: ourBaseHpBar.verticalCenter
            anchors.right: ourBaseHpBar.left
            anchors.rightMargin: 10

            maxValue: root.ourOutpostMaxHp
            slantWidth: -10
            barFromRight: true

            width: 200

            radiusBL: 5
        }

        HpBar {
            id: theirOutpostHpBar

            isInvincible: root.theirOutpostStatus === BaseMeta.OutpostStatus.INVINCIBLE

            targetHp: root.theirOutpostHp
            baseColor: !root.isOurRed ? root.redColor : root.blueColor
            invincibleColor: root.metallicGold

            bgColor: Qt.alpha(root.grayColor, 0.6)

            anchors.verticalCenter: theirBaseHpBar.verticalCenter
            anchors.left: theirBaseHpBar.right
            anchors.leftMargin: 10

            maxValue: root.theirOutpostMaxHp
            slantWidth: 10
            textAlignment: Qt.AlignRight

            width: 200

            radiusBR: 5
        }

        OutPostStatusIcon {
            id: ourOutpostStatusIcon

            baseColor: root.isOurRed ? root.redColor : root.blueColor

            anchors.top: ourOutpostHpBar.bottom
            anchors.left: ourOutpostHpBar.left
            anchors.topMargin: 5
            anchors.leftMargin: 14

            status: root.ourOutpostStatus

            slantWidth: -8
            gradientPreset: GradientPreset.LighterOnRight
        }

        OutPostStatusIcon {
            id: theirOutpostStatusIcon

            baseColor: !root.isOurRed ? root.redColor : root.blueColor

            anchors.top: theirOutpostHpBar.bottom
            anchors.right: theirOutpostHpBar.right
            anchors.topMargin: 5
            anchors.rightMargin: 14

            status: root.theirOutpostStatus

            slantWidth: 8
            gradientPreset: GradientPreset.LighterOnLeft
        }

        HGradShowOnValParalBadge {
            id: ourOutpostDefBadge

            defVal: root.ourOutpostDef

            baseColor: root.isOurRed ? root.redColor : root.blueColor

            anchors.top: ourOutpostHpBar.bottom
            anchors.left: ourOutpostStatusIcon.right
            anchors.topMargin: 5
            anchors.leftMargin: -5

            width: 70

            slantWidth: -8
        }

        HGradShowOnValParalBadge {
            id: theirOutpostDefBadge

            defVal: root.theirOutpostDef

            baseColor: !root.isOurRed ? root.redColor : root.blueColor

            anchors.top: theirOutpostHpBar.bottom
            anchors.right: theirOutpostStatusIcon.left
            anchors.topMargin: 5
            anchors.rightMargin: -5

            width: 70

            slantWidth: 8

            lighterOnLeft: true
        }
    }
}
