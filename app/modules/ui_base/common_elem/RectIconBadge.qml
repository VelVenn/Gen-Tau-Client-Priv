import QtQuick

import Gentau.Foundation
import Gentau.BasicWidgets

RectBadge {
    id: root

    property bool iconOnLeft: true
    property real badgeTextOffsetX: 15

    // Inherited from RectBadge
    gradientPreset: GradientPreset.NoPreset
    baseColor: 'gray'

    property string iconIdx: '\ue838' // star
    property real iconOffsetX: 10
    property real iconOffsetY: 0

    property alias iconFont: icon.font
    property alias iconColor: icon.iconColor

    borderWidth: 0

    textAlignment: Qt.AlignHCenter

    text: 'text'
    textOffsetX: iconOnLeft ? badgeTextOffsetX : -badgeTextOffsetX
    textOffsetY: 1

    verticalPadding: 0

    width: 100

    FontIcon {
        id: icon

        anchors.verticalCenter: root.verticalCenter
        anchors.verticalCenterOffset: root.iconOffsetY

        anchors.left: root.iconOnLeft ? root.left : undefined
        anchors.right: !root.iconOnLeft ? root.right : undefined
        anchors.leftMargin: root.iconOnLeft ? root.iconOffsetX : 0
        anchors.rightMargin: !root.iconOnLeft ? root.iconOffsetX : 0

        iconIdx: root.iconIdx

        font.pixelSize: root.font.pixelSize
    }
}
