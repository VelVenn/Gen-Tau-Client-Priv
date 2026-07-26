pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Effects

import Gentau.Foundation
import Gentau.CommonElem

Item {
    id: root

    property int leftAmmo: 0
    property real ammoSpd: 0.0

    property real badgeGap: 200.0

    property real scaleFactor: 1.0

    implicitWidth: content.childrenRect.width * scaleFactor
    implicitHeight: content.childrenRect.height * scaleFactor

    QtObject {
        id: param

        property real displaySpd: root.ammoSpd.toFixed(1)

        property color bgColor: Qt.alpha(Style.grayColor, 0.5)
        property color shawdowColor: Qt.alpha('black', 0.8)

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

        Rectangle {
            id: centerRect

            anchors.centerIn: pivot

            color: 'transparent'
            border.width: 0

            height: 10
            width: root.badgeGap
        }

        RectIconBadge {
            id: spdBadge

            anchors.verticalCenter: centerRect.verticalCenter
            anchors.right: centerRect.left

            width: 95
            height: 25

            borderRadius: 10

            font.family: Style.oxaniumFL.font.family
            font.pixelSize: 20

            text: param.displaySpd.toString()
            textColor: param.spdTextColor
            textOffsetY: 0

            iconIdx: '\ue9e4'
            iconColor: param.iconColor

            iconOffsetX: 5
            iconOffsetY: -1

            bgColor: param.bgColor

            layer.enabled: true
            layer.effect: MultiEffect {
                shadowEnabled: true
                shadowColor: param.shawdowColor

                shadowBlur: 0.8

                shadowHorizontalOffset: 5
                shadowVerticalOffset: 5

                autoPaddingEnabled: true
            }
        }

        RectMdiIconBadge {
            id: ammoBadge

            anchors.verticalCenter: centerRect.verticalCenter
            anchors.left: centerRect.right

            width: spdBadge.width
            height: spdBadge.height

            borderRadius: spdBadge.borderRadius

            font.family: spdBadge.font.family
            font.pixelSize: 20

            text: root.leftAmmo.toString()
            textColor: param.ammoTextColor
            textOffsetY: 1

            iconGlyph: '󰳨'
            iconColor: param.iconColor

            iconOnLeft: false
            iconOffsetX: 5

            bgColor: param.bgColor

            layer.enabled: true
            layer.effect: MultiEffect {
                shadowEnabled: true
                shadowColor: param.shawdowColor

                shadowBlur: 0.8

                shadowHorizontalOffset: -5
                shadowVerticalOffset: 5

                autoPaddingEnabled: true
            }
        }
    }
}
