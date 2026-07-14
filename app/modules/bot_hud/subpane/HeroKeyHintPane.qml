import QtQuick

import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Basic
import QtQuick.Effects

import Gentau.Foundation
import Gentau.BotHud.Element

Item {
    id: root

    property real scaleFactor: 1.0

    property bool isDeployVt: false
    property bool isDeployMode: false

    property alias deployModeProgress: deployModeKeyBadge.pressProgress

    property bool isJPressed: false
    property bool isHPressed: false
    property bool isKPressed: false
    property bool isLPressed: false

    implicitWidth: content.childrenRect.width * scaleFactor
    implicitHeight: content.childrenRect.height * scaleFactor

    QtObject {
        id: param

        property real basePxSize: 13
        property real baseKeySize: 12

        property bool baseTextBold: false
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

                Label {
                    text: "按"
                    color: "white"
                    font.family: Style.notoSansSC.font.family
                    font.pixelSize: param.basePxSize
                    font.bold: param.baseTextBold
                }

                KeyBadge {
                    text: "J"
                    font.pixelSize: param.baseKeySize
                    isPressed: root.isJPressed
                }

                Label {
                    text: root.isDeployVt ? "切换官方图传" : "切换部署图传"
                    color: "white"
                    font.family: Style.notoSansSC.font.family
                    font.pixelSize: param.basePxSize
                    font.bold: param.baseTextBold
                }

                Rectangle {
                    Layout.preferredWidth: 2
                    Layout.preferredHeight: 18
                    Layout.leftMargin: 6
                    Layout.rightMargin: 6
                    color: Qt.alpha("white", 0.5)
                }

                Label {
                    text: "按"
                    color: "white"
                    font.family: Style.notoSansSC.font.family
                    font.pixelSize: param.basePxSize
                    font.bold: param.baseTextBold

                    visible: root.isDeployVt
                }

                KeyBadge {
                    text: "H"
                    font.pixelSize: param.baseKeySize
                    isPressed: root.isJPressed

                    visible: root.isDeployVt
                }

                Label {
                    text: "重启部署图传"
                    color: "white"
                    font.family: Style.notoSansSC.font.family
                    font.pixelSize: param.basePxSize
                    font.bold: param.baseTextBold

                    visible: root.isDeployVt
                }

                Rectangle {
                    Layout.preferredWidth: 2
                    Layout.preferredHeight: 18
                    Layout.leftMargin: 6
                    Layout.rightMargin: 6
                    color: Qt.alpha("white", 0.5)

                    visible: root.isDeployVt
                }

                Label {
                    text: "长按"
                    color: "white"
                    font.family: Style.notoSansSC.font.family
                    font.pixelSize: param.basePxSize
                    font.bold: param.baseTextBold

                    visible: root.isDeployVt
                }

                KeyBadge {
                    id: deployModeKeyBadge

                    text: root.isDeployMode ? "L" : "K"
                    font.pixelSize: param.baseKeySize

                    isPressed: root.isDeployMode ? root.isLPressed : root.isKPressed

                    interactMode: KeyBadge.Mode.Delay
                }

                Label {
                    text: root.isDeployMode ? "退出部署模式" : "进入部署模式"
                    color: "white"
                    font.family: Style.notoSansSC.font.family
                    font.pixelSize: param.basePxSize
                    font.bold: param.baseTextBold
                }
            }
        }
    }
}
