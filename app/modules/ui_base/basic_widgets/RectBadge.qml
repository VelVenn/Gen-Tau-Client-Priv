import QtQuick

import Gentau.Foundation

Item {
    id: root

    property alias text: badgeText.text
    property alias font: badgeText.font
    property alias textColor: badgeText.color
    property int textAlignment: Qt.AlignHCenter

    property color bgColor: "#F0F0F0"
    property color borderColor: Qt.darker(bgColor, 1.2)
    property real borderWidth: 1.0
    property real borderRadius: 0.0

    // Shared gradient preset support. Keep default as NoPreset to preserve legacy bgColor rendering.
    property int gradientPreset: GradientPreset.NoPreset
    property color baseColor: bgColor

    property alias badgeGradient: bgRect.gradient

    readonly property Gradient presetGradient: {
        if (gradientPreset === GradientPreset.LighterOnLeft) {
            return param.lighterOnLeft
        }
        if (gradientPreset === GradientPreset.LighterOnRight) {
            return param.lighterOnRight
        }
        if (gradientPreset === GradientPreset.LighterOnTop || gradientPreset === GradientPreset.LighterOnUp) {
            return param.lighterOnTop
        }
        if (gradientPreset === GradientPreset.LighterOnBottom || gradientPreset === GradientPreset.LighterOnDown) {
            return param.lighterOnBottom
        }
        if (gradientPreset === GradientPreset.LighterCenter) {
            return param.lighterCenter
        }
        return null
    }

    property real radiusTL: borderRadius
    property real radiusTR: borderRadius
    property real radiusBR: borderRadius
    property real radiusBL: borderRadius

    property real horizontalPadding: 8.0
    property real verticalPadding: 4.0
    property real textOffsetY: 0.0
    property real textOffsetX: 0.0

    implicitWidth: badgeText.implicitWidth + (horizontalPadding * 2)
    implicitHeight: badgeText.implicitHeight + (verticalPadding * 2)

    QtObject {
        id: param

        property Gradient lighterOnLeft: Gradient {
            orientation: Gradient.Horizontal

            GradientStop { position: 0.0; color: Qt.alpha(root.baseColor, 0.3) }
            GradientStop { position: 0.4; color: Qt.alpha(root.baseColor, 0.5) }
            GradientStop { position: 0.8; color: Qt.alpha(root.baseColor, 0.8) }
            GradientStop { position: 1.0; color: Qt.alpha(root.baseColor, 1.0) }
        }

        property Gradient lighterOnRight: Gradient {
            orientation: Gradient.Horizontal

            GradientStop { position: 0.0; color: Qt.alpha(root.baseColor, 1.0) }
            GradientStop { position: 0.2; color: Qt.alpha(root.baseColor, 0.8) }
            GradientStop { position: 0.6; color: Qt.alpha(root.baseColor, 0.5) }
            GradientStop { position: 1.0; color: Qt.alpha(root.baseColor, 0.3) }
        }

        property Gradient lighterCenter: Gradient {
            orientation: Gradient.Horizontal

            GradientStop { position: 0.0; color: Qt.alpha(root.baseColor, 0.8) }
            GradientStop { position: 0.2; color: Qt.alpha(root.baseColor, 0.6) }
            GradientStop { position: 0.5; color: Qt.alpha(root.baseColor, 0.4) }
            GradientStop { position: 0.8; color: Qt.alpha(root.baseColor, 0.6) }
            GradientStop { position: 1.0; color: Qt.alpha(root.baseColor, 0.8) }
        }

        property Gradient lighterOnTop: Gradient {
            orientation: Gradient.Vertical

            GradientStop { position: 0.0; color: Qt.alpha(root.baseColor, 0.3) }
            GradientStop { position: 0.4; color: Qt.alpha(root.baseColor, 0.5) }
            GradientStop { position: 0.8; color: Qt.alpha(root.baseColor, 0.8) }
            GradientStop { position: 1.0; color: Qt.alpha(root.baseColor, 1.0) }
        }

        property Gradient lighterOnBottom: Gradient {
            orientation: Gradient.Vertical

            GradientStop { position: 0.0; color: Qt.alpha(root.baseColor, 1.0) }
            GradientStop { position: 0.2; color: Qt.alpha(root.baseColor, 0.8) }
            GradientStop { position: 0.6; color: Qt.alpha(root.baseColor, 0.5) }
            GradientStop { position: 1.0; color: Qt.alpha(root.baseColor, 0.3) }
        }
    }

    Rectangle {
        id: bgRect
        anchors.fill: parent
        color: root.bgColor
        gradient: root.presetGradient

        radius: root.borderRadius
        topLeftRadius: root.radiusTL
        topRightRadius: root.radiusTR
        bottomLeftRadius: root.radiusBL
        bottomRightRadius: root.radiusBR

        border.color: root.borderColor
        border.width: root.borderWidth
        antialiasing: true
    }

    Text {
        id: badgeText

        anchors.left: root.textAlignment === Qt.AlignLeft ? parent.left : undefined
        anchors.right: root.textAlignment === Qt.AlignRight ? parent.right : undefined
        anchors.horizontalCenter: root.textAlignment === Qt.AlignHCenter ? parent.horizontalCenter : undefined

        anchors.leftMargin: root.textAlignment === Qt.AlignLeft ? root.textOffsetX : 0
        anchors.rightMargin: root.textAlignment === Qt.AlignRight ? root.textOffsetX : 0

        anchors.horizontalCenterOffset: root.textOffsetX

        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: root.textOffsetY

        text: "text"
        color: "#333333"
        font.family: Style.notoSansSC.font.family
        font.pixelSize: 16.0
        font.weight: Font.Medium

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
