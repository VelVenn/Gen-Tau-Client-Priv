import QtQuick

import Gentau.Foundation
import Gentau.BasicWidgets

RectBadge {
    id: root

    property color baseColor: 'red'

    QtObject {
        id: param

        property Gradient gradRightLighter: Gradient {
            orientation: Gradient.Horizontal

            GradientStop { position: 0.0; color: Qt.alpha(root.baseColor, 1.0) }
            GradientStop { position: 0.2; color: Qt.alpha(root.baseColor, 0.8) }
            GradientStop { position: 0.6; color: Qt.alpha(root.baseColor, 0.5) }
            GradientStop { position: 0.8; color: Qt.alpha(root.baseColor, 0.3) }
            GradientStop { position: 1.0; color: Qt.alpha(root.baseColor, 0.1) }
        }

        property Gradient gradLeftLighter: Gradient {
            orientation: Gradient.Horizontal

            GradientStop { position: 0.0; color: Qt.alpha(root.baseColor, 0.1) }
            GradientStop { position: 0.2; color: Qt.alpha(root.baseColor, 0.3) }
            GradientStop { position: 0.6; color: Qt.alpha(root.baseColor, 0.5) }
            GradientStop { position: 0.8; color: Qt.alpha(root.baseColor, 0.8) }
            GradientStop { position: 1.0; color: Qt.alpha(root.baseColor, 1.0) }
        }
    }

    text: 'text'
    textColor: 'white'
    textAlignment: Qt.AlignLeft

    borderWidth: 0

    font.family: Style.firaCodeFL.font.family

    badgeGradient: textAlignment === Qt.AlignLeft ? param.gradRightLighter : param.gradLeftLighter

    Behavior on baseColor {
        enabled: root.visible

        ColorAnimation {
            duration: 200
            easing.type: Easing.OutQuad
        }
    }
}
