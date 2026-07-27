import QtQuick

import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Basic
import QtQuick.Effects

import Gentau.Foundation
import Gentau.HeroHud.Element

Item {
    id: root

    property real scaleFactor: 1.0

    property bool alwaysShow: true

    property bool isDeployVt: false
    property bool isDeployMode: false

    property alias deployModeProgress: deployModeHint.pressProgress

    property bool isJPressed: false
    property bool isHPressed: false
    property bool isKPressed: false
    property bool isLPressed: false

    implicitWidth: content.childrenRect.width * scaleFactor
    implicitHeight: content.childrenRect.height * scaleFactor

    component HintSeparator: Rectangle {
        Layout.preferredWidth: 2
        Layout.preferredHeight: 18
        Layout.leftMargin: 6
        Layout.rightMargin: 6

        color: Qt.alpha("white", 0.5)
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

        GroupBox {
            anchors.centerIn: pivot
            
            topPadding: 3
            bottomPadding: 3
            leftPadding: 10
            rightPadding: 10

            background: Rectangle {
                id: bg

                visible: root.alwaysShow || root.deployModeProgress > 0.0

                color: Qt.alpha(Style.grayColor, 0.4)

                border.width: 0

                radius: 5

                layer.enabled: true
                layer.effect: MultiEffect {
                    shadowEnabled: true
                    shadowColor: Qt.alpha('black', 0.8)

                    shadowBlur: 0.8

                    shadowVerticalOffset: 5

                    autoPaddingEnabled: true
                }
            }

            RowLayout {
                spacing: 8

                KeyHint {
                    keyTexts: ["J"]
                    hintText: root.isDeployVt ? "切换官方图传" : "切换部署图传"
                    isPressed: root.isJPressed

                    visible: root.alwaysShow && !root.isDeployMode
                }

                HintSeparator {
                    visible: root.alwaysShow && !root.isDeployMode
                }

                KeyHint {
                    keyTexts: ["H"]
                    hintText: "重启部署图传"
                    isPressed: root.isHPressed

                    visible: root.isDeployVt && root.alwaysShow
                }

                HintSeparator {
                    visible: root.isDeployVt && root.alwaysShow
                }

                KeyHint {
                    id: deployModeHint

                    actionText: "长按"
                    keyTexts: [root.isDeployMode ? "L" : "K"]
                    hintText: root.isDeployMode ? "退出部署模式" : "进入部署模式"

                    isPressed: root.isDeployMode ? root.isLPressed : root.isKPressed

                    interactMode: KeyHint.Mode.Delay

                    visible: root.alwaysShow || root.deployModeProgress > 0.0
                }
            }
        }
    }
}
