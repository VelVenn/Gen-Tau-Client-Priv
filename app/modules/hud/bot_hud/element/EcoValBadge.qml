import QtQuick

import Gentau.Foundation
import Gentau.BasicWidgets

RectBadge {
    id: root

    property real ecoVal: 0
    property color baseColor: 'red'

    QtObject {
        id: param

        property real displayEco: {
            if (root.ecoVal <= 0) { return 0 }

            return Math.max(1, Math.floor(root.ecoVal))
        }
    }

    badgeGradient: Gradient {
        orientation: Gradient.Horizontal

        GradientStop { position: 0.0; color: Qt.alpha(root.baseColor, 0.8) }
        GradientStop { position: 0.2; color: Qt.alpha(root.baseColor, 0.6) }
        GradientStop { position: 0.5; color: Qt.alpha(root.baseColor, 0.4) }
        GradientStop { position: 0.8; color: Qt.alpha(root.baseColor, 0.6) }
        GradientStop { position: 1.0; color: Qt.alpha(root.baseColor, 0.8) }
    }

    width: 150
    height: 45

    borderRadius: 10

    text: param.displayEco.toString()
    textColor: 'white'
    textAlignment: Qt.AlignHCenter
    textOffsetX: 20

    borderWidth: 0

    font.family: Style.orbitronFL.font.family
    font.pixelSize: 30

    Behavior on ecoVal {
        enabled: root.visible

        NumberAnimation {
            duration: 200
            easing.type: Easing.OutQuad
        }
    }

    FontIcon {
        anchors.verticalCenter: root.verticalCenter
        anchors.left: root.left

        anchors.leftMargin: 0

        iconIdx: '\uf49b' // poker clip

        font.pixelSize: root.height * 0.9
        font.weight: 200

        iconColor: 'white'
    }
}
