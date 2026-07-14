import QtQuick
import QtQuick.Shapes

import Gentau.BasicWidgets

ParalBadge {
    id: root

    property color baseColor: 'red'
    property bool lighterOnLeft: false

    QtObject {
        id: param
        property real startX: root.lighterOnLeft ? root.width : 0
        property real endX: root.lighterOnLeft ? 0 : root.width
    }

    slantWidth: -10

    text: 'text'
    textColor: 'white'
    verticalPadding: 0
    textOffsetY: 1

    font.family: 'Fira Code'
    font.pixelSize: 20

    borderWidth: 0
    borderRadius: 0

    badgeGradient: LinearGradient {
        x1: param.startX; y1: 0
        x2: param.endX; y2: 0

        GradientStop { position: 0.0; color: Qt.alpha(root.baseColor, 1.0) }
        GradientStop { position: 0.2; color: Qt.alpha(root.baseColor, 0.8) }
        GradientStop { position: 0.6; color: Qt.alpha(root.baseColor, 0.5) }
        GradientStop { position: 1.0; color: Qt.alpha(root.baseColor, 0.3) }
    }

    Behavior on baseColor {
        enabled: root.visible

        ColorAnimation {
            duration: 250
            easing.type: Easing.InOutQuad
        }
    }
}
