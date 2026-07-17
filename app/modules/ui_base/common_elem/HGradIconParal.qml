import QtQuick

import Gentau.Foundation
import Gentau.BasicWidgets

Paral {
    id: root

    property alias iconIndex: icon.iconIdx
    property alias iconColor: icon.iconColor
    property alias iconScale: icon.scale
    property alias iconRotation: icon.rotation
    property alias font: icon.font

    property real horizontalPadding: 8.0
    property real verticalPadding: 4.0
    property real iconOffsetX: 0.0
    property real iconOffsetY: 0.0

    gradientPreset: GradientPreset.LighterOnLeft

    implicitWidth: icon.implicitWidth + (horizontalPadding * 2) + Math.abs(root.slantWidth)
    implicitHeight: icon.implicitHeight + (verticalPadding * 2)

    FontIcon {
        id: icon

        anchors.centerIn: parent
        anchors.horizontalCenterOffset: root.iconOffsetX
        anchors.verticalCenterOffset: root.iconOffsetY

        iconColor: 'white'
    }
}
