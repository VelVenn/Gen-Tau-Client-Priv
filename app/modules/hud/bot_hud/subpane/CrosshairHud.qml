import QtQuick

import Gentau.Foundation
import Gentau.BasicWidgets

Item {
    id: root

    property real scaleFactor: 1.0

    property real heatVal: 0
    property real maxHeat: 100
    property real crossSize: 50

    property bool isOverheat: false

    implicitWidth: (content.childrenRect.width) * scaleFactor
    implicitHeight: (content.childrenRect.height) * scaleFactor

    layer.enabled: true
    layer.samples: 4

    antialiasing: true

    QtObject {
        id: param

        readonly property int sizePadding: 10

        property color bgRingColor: Qt.alpha(Style.grayColor, 0.6)

        property color baseColor: Style.lightFire

        readonly property real ringOp: 1

        property color ringColor: {
            if (heatProg.progress < 0.25) {
                return Qt.alpha(Style.lightGreen, param.ringOp)
            } else if (heatProg.progress < 0.5) {
                return Qt.alpha(Style.lightDirt, param.ringOp)
            } else if (heatProg.progress < 0.75) {
                return Qt.alpha(Style.lightCrimson, param.ringOp)
            } else {
                return Qt.alpha(Style.lightFire, param.ringOp)
            }
        }
    }

    onIsOverheatChanged: {
        overheatBadge.visible = root.isOverheat
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

        CircularProgressBar {
            id: heatProg

            anchors.centerIn: pivot

            property real targetVal: root.heatVal

            property bool maxValJustChanged: false

            value: 0
            maxValue: root.maxHeat

            bgColor: param.bgRingColor

            ringWidth: 3
            bgRingWidth: ringWidth * 2

            radius: 60

            fillColor: param.ringColor

            Behavior on fillColor {
                id: colorAnime

                enabled: root.visible && !heatProg.maxValJustChanged

                ColorAnimation {
                    duration: 150
                    easing.type: Easing.OutQuad
                }
            }

            Behavior on value {
                id: valAnime

                enabled: root.visible && !heatProg.maxValJustChanged

                NumberAnimation {
                    duration: 150
                    easing.type: Easing.Linear
                }
            }

            onMaxValueChanged: {
                maxValJustChanged = true

                value = targetVal

                maxValJustChanged = false
            }

            onTargetValChanged: {
                value = targetVal
            }
        }

        Image {
           id: crossImg

           asynchronous: true

           anchors.centerIn: pivot

           visible: !root.isOverheat

           source: 'images/crossPaleGreen.svg'

           width: root.crossSize
           height: width
        }

        RectBadge {
            id: overheatBadge

            anchors.centerIn: pivot
            anchors.verticalCenterOffset: 1

            text: 'OVERHEAT'
            textColor: 'white'
            textOffsetY: 1

            font.family: Style.oxaniumFL.font.family

            borderRadius: 5
            borderWidth: 0

            badgeGradient: Gradient {
                orientation: Gradient.Horizontal

                GradientStop { position: 0.0; color: Qt.alpha(param.baseColor, 0.8) }
                GradientStop { position: 0.2; color: Qt.alpha(param.baseColor, 0.6) }
                GradientStop { position: 0.5; color: Qt.alpha(param.baseColor, 0.4) }
                GradientStop { position: 0.8; color: Qt.alpha(param.baseColor, 0.6) }
                GradientStop { position: 1.0; color: Qt.alpha(param.baseColor, 0.8) }
            }

            verticalPadding: 2

            visible: false

            state: visible ? 'shown' : 'hidden'
            states: [
                State {
                    name: "shown"
                    PropertyChanges { /*target: overheatBadge; opacity: 1; scale: 1*/
                        overheatBadge.opacity: 1
                        overheatBadge.scale: 1
                    }
                },
                State {
                    name: "hidden"
                    PropertyChanges { /*target: overheatBadge; opacity: 0; scale: 0.8*/
                        overheatBadge.opacity: 0
                        overheatBadge.scale: 0.8
                    }
                }
            ]

            transitions: Transition {
                enabled: root.visible

                NumberAnimation {
                    properties: "opacity,scale"
                    duration: 100
                    easing.type: Easing.OutCubic
                }
            }
        }
    }
}
