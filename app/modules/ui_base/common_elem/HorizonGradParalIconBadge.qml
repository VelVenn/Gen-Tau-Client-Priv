import QtQuick
import QtQuick.Shapes

import Gentau.BasicWidgets

ParalBadge {
    id: root

    property color baseColor: 'red'
    property bool lighterOnLeft: false

    property bool iconOnLeft: true
    property real badgeTextOffsetX: 10

    property bool enableColorAnim: true

    property string iconIndex: '\ue838' // default as star
    property real iconOffsetX: 10
    property alias iconFont: icon.font

    QtObject {
        id: param
        property real startX: root.lighterOnLeft ? root.width : 0
        property real endX: root.lighterOnLeft ? 0 : root.width
    }

    text: 'text'
    textColor: 'white'
    textOffsetY: 1.5
    textOffsetX: iconOnLeft ? badgeTextOffsetX : -badgeTextOffsetX
    verticalPadding: 0

    font.family: Style.firaCodeFL.font.family
    font.pixelSize: 16

    borderRadius: 0
    borderColor: 'transparent'

    width: 100
    slantWidth: -8

    badgeGradient: LinearGradient {
        x1: param.startX; y1: 0
        x2: param.endX; y2: 0

        GradientStop { position: 0.0; color: Qt.alpha(root.baseColor, 1.0) }
        GradientStop { position: 0.2; color: Qt.alpha(root.baseColor, 0.8) }
        GradientStop { position: 0.6; color: Qt.alpha(root.baseColor, 0.5) }
        GradientStop { position: 1.0; color: Qt.alpha(root.baseColor, 0.3) }
    }

    FontIcon {
        id: icon

        iconIdx: root.iconIndex

        iconColor: root.textColor

        anchors.verticalCenter: root.verticalCenter
        anchors.verticalCenterOffset: 0

        anchors.left: root.iconOnLeft ? root.left : undefined
        anchors.right: !root.iconOnLeft ? root.right : undefined
        anchors.leftMargin: root.iconOnLeft ? root.iconOffsetX : 0
        anchors.rightMargin: !root.iconOnLeft ? root.iconOffsetX : 0

        font.pixelSize: root.font.pixelSize
    }

    Behavior on baseColor {
        enabled: root.visible && root.enableColorAnim

        ColorAnimation {
            duration: 250
            easing.type: Easing.InOutQuad
        }
    }
}
