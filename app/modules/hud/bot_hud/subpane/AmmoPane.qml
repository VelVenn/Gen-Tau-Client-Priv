pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Effects
import QtQuick.Controls.Material

import Gentau.Foundation
import Gentau.BasicWidgets
import Gentau.BotHud.Element

Item {
    id: root

    property alias bgWidth: ammoBadge.width
    property alias bgHeight: ammoBadge.height

    property int leftAmmo: 0
    property real ammoSpd: 0.0

    property bool shadowOnLeft: true

    property real scaleFactor: 1.0

    implicitWidth: content.childrenRect.width * scaleFactor
    implicitHeight: content.childrenRect.height * scaleFactor

    QtObject {
        id: param

        property color bgColor: Style.grayColor

        property color ammoTextColor: {
            if (root.leftAmmo <= 0) {
                return Style.lightFire
            }

            return Style.lightGreen
        }

        property color spdTextColor: Style.lightGreen

        property color iconColor: Style.lightGreen
    }

    Behavior on leftAmmo {
        enabled: root.visible

        NumberAnimation {
            duration: 150
            easing.type: Easing.Linear
        }
    }

    Behavior on ammoSpd {
        enabled: root.visible

        NumberAnimation {
            duration: 150
            easing.type: Easing.Linear
        }
    }

    layer.enabled: true
    layer.effect: MultiEffect{
        shadowEnabled: true
        shadowColor: Qt.rgba(0,0,0,0.8)

        shadowBlur: 0.8

        shadowHorizontalOffset: root.shadowOnLeft ? -5 : 5
        shadowVerticalOffset: 5

        // 这个属性会让光晕自由扩散，并且不会被矩形边界像切豆腐一样切成硬直角死边！
        autoPaddingEnabled: true
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

        RectBadge {
            id: ammoBadge

            anchors.horizontalCenter: pivot.horizontalCenter
            anchors.top: pivot.top

            width: 95
            height: 25

            radiusTL: 10
            radiusTR: 10
            borderWidth: 0

            font.family: Style.oxaniumFL.font.family
            font.pixelSize: 20

            textAlignment: Qt.AlignHCenter

            text: root.leftAmmo.toString()
            textColor: param.ammoTextColor
            textOffsetX: 15
            textOffsetY: 1

            bgColor: Qt.alpha(param.bgColor, 0.5)

            MdiFontIcon {
                anchors.verticalCenter: ammoBadge.verticalCenter
                anchors.left: ammoBadge.left

                anchors.leftMargin: 5

                text: '󰳨'
                iconColor: param.iconColor

                font.pixelSize: 20
            }
        }

        RectBadge {
            id: speedBadge

            anchors.horizontalCenter: pivot.horizontalCenter
            anchors.top: ammoBadge.bottom

            anchors.topMargin: -0.05

            width: ammoBadge.width
            height: ammoBadge.height

            borderWidth: 0

            radiusBL: ammoBadge.radiusTL
            radiusBR: ammoBadge.radiusTR

            bgColor: Qt.alpha(param.bgColor, 0.5)

            font.family: Style.oxaniumFL.font.family
            font.pixelSize: 20

            text: root.ammoSpd.toFixed(1).toString()
            textColor: param.spdTextColor
            textOffsetX: 15
            textOffsetY: 1

            FontIcon {
                anchors.verticalCenter: speedBadge.verticalCenter
                anchors.left: speedBadge.left

                anchors.leftMargin: 5

                iconIdx: '\ue9e4'
                iconColor: param.iconColor

                font.pixelSize: 20
                font.weight: 400
            }
        }
    }
}
