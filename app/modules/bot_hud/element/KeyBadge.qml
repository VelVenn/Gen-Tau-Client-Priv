pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Shapes
import QtQuick.Effects

import Gentau.Foundation
import Gentau.BasicWidgets

RectBadge {
    id: root

    property color baseColor: Qt.alpha(Qt.lighter(Style.grayBlue, 1.2), 0.8)

    property int minWidth: 22

    borderColor: Qt.darker(root.baseColor, 1.2)
    borderWidth: 1
    borderRadius: 4
    textColor: "white"

    horizontalPadding: 4
    verticalPadding: 3

    font.family: Style.notoSansSC.font.family
    font.pixelSize: 14
    font.bold: true

    Layout.minimumWidth: root.minWidth
    Layout.preferredWidth: Math.max(root.minWidth, implicitWidth)

    enum Mode {
        Click,
        Delay
    }

    property int interactMode: KeyBadge.Mode.Click

    property bool isPressed: false
    property int animDuration: 1000 // 仅在 Delay 模式下生效
    property real pressProgress: 0.0 // 仅供 Delay 模式渲染使用

    readonly property real displayProg: {
        if (root.interactMode === KeyBadge.Mode.Delay) {
            return 0
        }

        if (!root.isPressed) {
            return 0
        }

        return Math.min(1, Math.max(0, root.pressProgress))
    }
    
    property color progressColor: Qt.rgba(1, 1, 1, 0.6)

    bgColor: (root.isPressed && root.interactMode !== KeyBadge.Mode.Delay) ? Qt.darker(root.baseColor, 1.3) : root.baseColor
    Behavior on bgColor {
        enabled: root.visible

        ColorAnimation { duration: 150 }
    }

    Item {
        id: sweepContainer
        anchors.fill: parent
        anchors.margins: root.borderWidth
        visible: root.interactMode === KeyBadge.Mode.Delay && (root.isPressed || root.displayProg > 0)
        clip: true

        layer.enabled: visible
        layer.effect: MultiEffect {
            maskEnabled: true
            maskSource: maskRect
        }

        Shape {
            id: sweepShape
            anchors.centerIn: parent
            width: Math.sqrt(parent.width*parent.width + parent.height*parent.height) * 2
            height: width

            ShapePath {
                fillColor: root.progressColor
                strokeColor: "transparent"

                startX: sweepShape.width / 2
                startY: sweepShape.height / 2

                PathLine { x: sweepShape.width / 2; y: 0 }

                PathAngleArc {
                    centerX: sweepShape.width / 2
                    centerY: sweepShape.height / 2
                    radiusX: sweepShape.width / 2
                    radiusY: sweepShape.height / 2
                    startAngle: -90
                    sweepAngle: 360 * root.displayProg
                }

                PathLine { x: sweepShape.width / 2; y: sweepShape.height / 2 }
            }
        }
    }

    Rectangle {
        id: maskRect
        width: sweepContainer.width
        height: sweepContainer.height
        radius: Math.max(2, root.borderRadius)
        color: "black"
        layer.enabled: true
        visible: false
    }

    // NumberAnimation on pressProgress {
    //     running: root.interactMode === KeyBadge.Mode.Delay && root.isPressed
    //     from: 0.0
    //     to: 1.0
    //     duration: root.animDuration
    // }
    
    // onIsPressedChanged: {
    //     if (!root.isPressed) {
    //         root.pressProgress = 0.0
    //     }
    // }
}
