import QtQuick
import QtQuick.Effects

import Gentau.Foundation
import Gentau.BasicWidgets
import Gentau.CommonElem
import Gentau.BotHud.Element

import "avatarAtlasSprite.js" as AtlasInfo

Item {
    id: root

    property int botIdx: 0
    property int botLv: 0

    property int botExp: 0
    property int botCurLvMaxExp: 0

    property real botHp: 250
    property real maxBotHp: 250

    property int botBufEnergy: 60
    property int botChassisEnergy: 40000

    property bool isInvincible: false

    property bool isOffline: false

    property real scaleFactor: 1.0

    implicitWidth: content.childrenRect.width * scaleFactor
    implicitHeight: content.childrenRect.height * scaleFactor

    layer.enabled: true
    layer.samples: 4

    QtObject {
        id: param

        property int camp: BotMeta.toBotCamp(root.botIdx)

        property string botRealIdxStr: {
            var realIdx = BotMeta.toRealBotIdx(root.botIdx)

            if (realIdx > 0)
                return realIdx.toString();

            return '';
        }

        property string avatarIdx: BotMeta.toBotAvatarIdxString(root.botIdx)

        property int displayMaxHp: {
            if (root.maxBotHp <= 0) { return 1 }

            return Math.max(1, Math.floor(root.maxBotHp))
        }

        property string hpStr: botHpBar.displayHp.toString() + ' / ' + displayMaxHp.toString()

        readonly property int maxBufEnergy: 60
        readonly property int maxChassisEnergy: 40000

        property color baseColor: {
            switch (camp) {
            case BotMeta.BotCamp.RED:
                return Style.redColor
            case BotMeta.BotCamp.BLUE:
                return Style.blueColor
            default:
                return Style.grayColor
            }
        }

        property color bufBarColor: Style.lightCrimson
        property color chasBarColor: Style.lightDirt

        Behavior on baseColor {
            enabled: root.visible

            ColorAnimation {
                duration: 200
                easing.type: Easing.InOutQuad
            }
        }
    }

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

        SpriteAvatar {
            id: botAvatar

            width: 120
            height: 120

            imgSrc: Qt.resolvedUrl(AtlasInfo.atlasData.src)

            roiX: AtlasInfo.atlasData.frames[param.avatarIdx]?.x ?? 0
            roiY: AtlasInfo.atlasData.frames[param.avatarIdx]?.y ?? 0
            roiW: AtlasInfo.atlasData.frames[param.avatarIdx]?.w ?? 120
            roiH: AtlasInfo.atlasData.frames[param.avatarIdx]?.h ?? 120

            imgScale: AtlasInfo.atlasData.frames[param.avatarIdx]?.scale ?? 1.0

            border.width: 2
            border.color: param.baseColor

            bgColor: Style.grayBlue
        }

        FontIcon {
            id: offlineIcon

            anchors.centerIn: botAvatar

            visible: root.isOffline && root.visible

            iconIdx: '\ue16f'
            iconColor: Style.lightFire

            font.pixelSize: 45

            rotation: -60

            SequentialAnimation {
                id: opaAnime

                running: offlineIcon.visible
                loops: Animation.Infinite

                NumberAnimation {
                    target: offlineIcon
                    property: 'opacity'
                    from: 1.0; to: 0.4
                    duration: 500
                    easing.type: Easing.InOutQuad
                }

                NumberAnimation {
                    target: offlineIcon
                    property: 'opacity'
                    from: 0.4; to: 1.0
                    duration: 500
                    easing.type: Easing.InOutQuad
                }
            }
        }

        RectBadge {
            id: idxBadge

            anchors.horizontalCenter: botAvatar.horizontalCenter
            anchors.bottom: botAvatar.bottom

            anchors.bottomMargin: -10

            width: 35
            height: width

            borderRadius: width
            borderColor: param.baseColor
            borderWidth: 2

            bgColor: Style.grayBlue

            font.family: Style.orbitronFL.font.family
            font.pixelSize: 20
            font.bold: true

            text: param.botRealIdxStr
            textOffsetX: -0.3
            textColor: 'white'
        }

        CircularProgressBar {
            id: lvBar

            anchors.centerIn: botAvatar

            property int targetVal: root.botExp
            property bool maxValueChanged: false

            maxValue: root.botCurLvMaxExp

            startDeg: 210
            endDeg: 320

            fillColor: Style.lightGreen
            bgColor: Qt.alpha(Style.grayColor, 0.8)

            ringWidth: 3
            bgRingWidth: ringWidth * 2

            radius: 70

            Behavior on value {
                id: valAnime

                enabled: root.visible && !lvBar.maxValueChanged

                NumberAnimation {
                    duration: 150
                    easing.type: Easing.Linear
                }
            }

            onMaxValueChanged: {
                maxValueChanged = true
                value = targetVal
                maxValueChanged = false
            }

            onTargetValChanged: {
                value = targetVal
            }
        }

        RectBadge {
            id: lvBadge

            anchors.top: botAvatar.top
            anchors.horizontalCenter: botAvatar.horizontalCenter

            anchors.topMargin: -10
            anchors.horizontalCenterOffset: -30

            width: 30
            height: width

            borderRadius: height
            borderColor: param.baseColor
            borderWidth: 2

            bgColor: Style.grayBlue

            font.family: Style.oxaniumFL.font.family
            font.pixelSize: 17

            text: root.botLv.toString()
            textOffsetY: 1
            textColor: 'white'

            layer.enabled: true
            layer.effect: MultiEffect{
                shadowEnabled: true
                shadowColor: Qt.rgba(0,0,0,0.8)

                shadowBlur: 0.8

                shadowHorizontalOffset: 5
                shadowVerticalOffset: 5

                autoPaddingEnabled: true
            }
        }

        HpBar {
            id: botHpBar

            anchors.bottom: botBufEnergyBar.top
            anchors.left: botAvatar.right

            anchors.bottomMargin: 8

            targetHp: root.botHp
            maxValue: root.maxBotHp

            baseColor: param.baseColor
            invincibleColor: Style.metallicGold
            bgColor: Qt.alpha(Style.grayColor, 0.6)

            isInvincible: root.isInvincible

            slantWidth: 10

            radiusTR: 5
            radiusBR: 5

            width: 300
            height: 28

            lowHpThreshold: 0.3

            font.pixelSize: 15

            text: param.hpStr
            textAlignment: Qt.AlignHCenter
        }

        HpBar {
            id: botBufEnergyBar

            anchors.bottom: botChassisEnergyBar.top
            anchors.left: botHpBar.left

            anchors.bottomMargin: 5
            anchors.leftMargin: -6

            width: 250
            height: 8

            targetHp: root.botBufEnergy
            maxValue: param.maxBufEnergy

            slantWidth: 3

            fillColor: param.bufBarColor
            bgColor: Qt.alpha(Style.grayColor, 0.5)

            displayText: false

            lowHpThreshold: 0.3
        }

        HpBar {
            id: botChassisEnergyBar

            anchors.bottom: botAvatar.bottom
            anchors.left: botBufEnergyBar.left

            anchors.leftMargin: -6
            anchors.bottomMargin: 2

            width: 250
            height: 8

            slantWidth: 3

            targetHp: root.botChassisEnergy
            maxValue: param.maxChassisEnergy

            fillColor: param.chasBarColor
            bgColor: Qt.alpha(Style.grayColor, 0.5)

            displayText: false

            lowHpThreshold: 0.3
        }
    }
}
