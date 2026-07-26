import QtQuick

import Gentau.Foundation
import Gentau.BasicWidgets

Item {
    id: root

    property alias isRed: progBar.isRed
    property alias value: progBar.value
    property alias maxValue: progBar.maxValue

    property alias radius: progBar.radius
    property alias ringWidth: progBar.ringWidth
    property alias bgRingWidth: progBar.bgRingWidth

    property alias badgeWidth: secBadge.width

    property real scaleFactor: 1.0

    implicitWidth: content.childrenRect.width * scaleFactor
    implicitHeight: content.childrenRect.height * scaleFactor

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

        CircularProgressBar {
            id: progBar

            property bool isRed: true

            anchors.horizontalCenter: pivot.horizontalCenter
            anchors.top: pivot.top

            radius: 25 - ringWidth
            ringWidth: 4

            fillColor: !isRed ? Style.redColor : Style.blueColor
            bgColor: Qt.alpha(Style.grayColor, 0.3)

            value: 0

            visible: value > 0

            state: value > 0 ? 'shown' : 'hidden'
            states: [
                State {
                    name: "shown"
                    PropertyChanges { /*target: progBar; opacity: 1; scale: 1 */
                        progBar.opacity: 1
                        progBar.scale: 1
                    }
                },
                State {
                    name: "hidden"
                    PropertyChanges { /*target: progBar; opacity: 0; scale: 0.8*/
                        progBar.opacity: 0
                        progBar.scale: 0.8
                    }
                }
            ]

            Behavior on value {
                enabled: progBar.visible

                NumberAnimation {
                    duration: 150
                    easing.type: Easing.Linear
                }
            }

            Behavior on fillColor {
                enabled: progBar.visible

                ColorAnimation {
                    duration: 200
                    easing.type: Easing.OutQuad
                }
            }

            transitions: Transition {
                NumberAnimation {
                    properties: "opacity,scale"
                    duration: 150
                    easing.type: Easing.OutCubic
                }
            }

            FontIcon {
                id: icon

                anchors.centerIn: parent

                iconIdx: '\ueb39' // Hexagon
                iconColor: root.isRed ? Style.redColor : Style.blueColor

                font.pixelSize: 28
                font.bold: true
            }
        }

        RectBadge {
            id: secBadge

            anchors.horizontalCenter: progBar.horizontalCenter
            anchors.top: progBar.bottom

            anchors.topMargin: 5

            badgeGradient: Gradient {
                orientation: Qt.Horizontal

                GradientStop { position: 0.0; color: Qt.alpha(progBar.fillColor, 0.8) }
                GradientStop { position: 0.2; color: Qt.alpha(progBar.fillColor, 0.6) }
                GradientStop { position: 0.5; color: Qt.alpha(progBar.fillColor, 0.4) }
                GradientStop { position: 0.8; color: Qt.alpha(progBar.fillColor, 0.6) }
                GradientStop { position: 1.0; color: Qt.alpha(progBar.fillColor, 0.8) }
            }

            borderWidth: 0
            borderRadius: 4

            verticalPadding: 0
            textOffsetY: 1

            width: 55

            visible: progBar.visible
            opacity: progBar.opacity
            scale: progBar.scale

            property real displayVal: {
                if (progBar.value <= 0) { return 0 }

                return Math.min(progBar.value, progBar.maxValue)
            }

            text: displayVal.toFixed(1) + 's'
            textColor: 'white'
            font.pixelSize: 13
            font.family: 'Fira Code'
            font.bold: true
        }
    }
}
