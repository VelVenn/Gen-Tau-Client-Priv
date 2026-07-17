import QtQuick
import QtQuick.Effects

import Gentau.BasicWidgets

Item {
    id: root

    // --- 状态 ---
    property bool isOn: false

    // --- 闪烁 ---
    property bool blinkEnabled: false      // On 状态下是否闪烁
    property int blinkInterval: 600        // 闪烁周期 (ms)

    // --- 指示灯颜色 ---
    property color onColor: Qt.alpha("#22DD44", 1.0)
    property color offColor: Qt.alpha('white', 0.4)

    // --- 底板 ---
    property color bgColor: Qt.alpha(Style.grayColor, 0.6)
    property color bgBorderColor: Qt.rgba(0.08, 0.08, 0.10, 0.85)
    property alias bgBorderWidth: bgPlate.border.width

    property bool bgShadowEnabled: true

    property real bgPadding: 2

    // --- Icon 属性 ---
    property string iconIdx: '\ue87d'
    property int iconSize: 15
    
    // 允许在背板中微调图标的相对位置（解决不同字体排版可能造成的视觉偏移）
    property real iconOffsetX: 0
    property real iconOffsetY: 0

    property bool lampShadowEnabled: true

    property alias lampFont: lamp.font

    // --- Glow 光晕 ---
    property real glowRadius: 8.0          // 光晕扩散半径
    property real glowOpacity: 0.7         // 光晕不透明度

    // --- 文本 ---
    property bool showText: false
    property string labelText: ""
    property color labelColor: "#D8D8D8"

    property alias labelFont: labelItem.font

    property alias labelBgColor: labelItem.bgColor
    property bool labelBgShadowEnabled: true

    property alias labelBorderColor: labelItem.borderColor
    property alias labelBorderWidth: labelItem.borderWidth
    property alias labelBorderRad: labelItem.borderRadius

    // --- 文本定位 ---
    property int textAnchor: Alignment.Flag.Right   // 九宫格锚点
    property real textOffsetX: 0.0                  // 锚点偏移 X
    property real textOffsetY: 0.0                  // 锚点偏移 Y

    property real scaleFactor: 1.0

    implicitWidth: content.childrenRect.width * scaleFactor
    implicitHeight: content.childrenRect.height * scaleFactor

    QtObject {
        id: param

        // 闪烁可见性标志（On + blinkEnabled 时交替翻转）
        property bool blinkVisible: true

        // 状态切换瞬间跳过 Behavior 动画
        property bool skipAnimation: false

        readonly property color activeColor: {
            if (!root.isOn)
                return root.offColor;
            if (root.blinkEnabled && !blinkVisible)
                return root.offColor;
            return root.onColor;
        }

        readonly property bool glowing: root.isOn && (!root.blinkEnabled || blinkVisible)
    }

    Timer {
        id: blinkTimer

        interval: root.blinkInterval
        repeat: true
        running: root.isOn && root.blinkEnabled && root.visible

        onTriggered: param.blinkVisible = !param.blinkVisible

        // 重置：状态切换时确保灯首帧可见
        onRunningChanged: {
            param.skipAnimation = true;
            if (running)
                param.blinkVisible = false;
            else
                param.blinkVisible = true;
            param.skipAnimation = false;
        }
    }

    Item {
        id: content

        transformOrigin: Item.TopLeft
        scale: root.scaleFactor
        x: -content.childrenRect.x * content.scale
        y: -content.childrenRect.y * content.scale

        // --- 底部暗色衬板 (底板方案) ---
        // 将 bgPlate 作为坐标原点，让灯在其中居中，这样调整灯的偏移时底板不会乱跑
        Rectangle {
            id: bgPlate
            x: 0
            y: 0
            
            width: Math.max(lamp.implicitWidth, lamp.implicitHeight) + root.bgPadding * 2
            height: width
            radius: width / 4
            
            color: root.bgColor
            border.width: 1.5
            border.color: root.bgBorderColor

            layer.enabled: root.bgShadowEnabled
            layer.effect: MultiEffect {
                shadowEnabled: true
                shadowColor: Qt.rgba(0, 0, 0, 0.8)
                shadowBlur: 0.5
                shadowVerticalOffset: 3
                shadowHorizontalOffset: 0
            }
        }

        // --- 光晕层 (Glow) ---
        Item {
            id: glowSourceContainer
            anchors.centerIn: lamp
            width: lamp.width + root.glowRadius * 2
            height: lamp.height + root.glowRadius * 2
            visible: false  // 仅作为 MultiEffect 源

            FontIcon {
                id: glowSource
                anchors.centerIn: parent

                iconIdx: lamp.iconIdx
                font: lamp.font

                iconColor: param.activeColor
            }
        }

        MultiEffect {
            id: glowEffect
            anchors.centerIn: lamp
            width: glowSourceContainer.width
            height: glowSourceContainer.height
            source: glowSourceContainer

            blurEnabled: true
            blurMax: 32
            blur: root.glowRadius / 32.0

            opacity: param.glowing ? root.glowOpacity : 0.0

            Behavior on opacity {
                enabled: root.blinkEnabled && !param.skipAnimation && root.visible
                NumberAnimation {
                    duration: 120
                    easing.type: Easing.OutQuad
                }
            }
        }

        FontIcon {
            id: lamp
            
            anchors.centerIn: bgPlate
            anchors.horizontalCenterOffset: root.iconOffsetX
            anchors.verticalCenterOffset: root.iconOffsetY
            
            iconIdx: root.iconIdx
            font.pixelSize: root.iconSize
            font.bold: true

            iconColor: param.activeColor

            // 灯色切换平滑过渡
            Behavior on iconColor {
                enabled: root.blinkEnabled && !param.skipAnimation && root.visible
                ColorAnimation {
                    duration: 100
                    easing.type: Easing.OutQuad
                }
            }

            layer.enabled: root.lampShadowEnabled
            layer.effect: MultiEffect {
                shadowEnabled: true
                shadowColor: Qt.rgba(0, 0, 0, 0.8)
                shadowBlur: 0.5
                shadowVerticalOffset: 2
                shadowHorizontalOffset: 0
            }
        }

        // --- 文本标签 ---
        RectBadge {
            id: labelItem

            visible: root.showText && root.labelText.length > 0

            text: root.labelText
            textColor: root.labelColor
            textOffsetY: 0

            horizontalPadding: 2
            verticalPadding: 1

            borderRadius: 5

            bgColor: Qt.alpha(Style.grayColor, 0.6)

            layer.enabled: root.labelBgShadowEnabled
            layer.effect: MultiEffect {
                shadowEnabled: true
                shadowColor: Qt.rgba(0, 0, 0, 0.8)
                shadowBlur: 0.5
                shadowVerticalOffset: 2
                shadowHorizontalOffset: 0
            }

            // ---- 九宫格锚点定位（相对 bgPlate，防止重叠） ----
            states: [
                State {
                    name: "topLeft"
                    when: root.textAnchor === Alignment.Flag.TopLeft
                    AnchorChanges {
                        target: labelItem
                        anchors.bottom: bgPlate.top
                        anchors.right: bgPlate.left
                    }
                    PropertyChanges {
                        labelItem.anchors.bottomMargin: root.textOffsetY
                        labelItem.anchors.rightMargin: root.textOffsetX
                    }
                },
                State {
                    name: "top"
                    when: root.textAnchor === Alignment.Flag.Top
                    AnchorChanges {
                        target: labelItem
                        anchors.bottom: bgPlate.top
                        anchors.horizontalCenter: bgPlate.horizontalCenter
                    }
                    PropertyChanges {
                        labelItem.anchors.bottomMargin: root.textOffsetY
                        labelItem.anchors.horizontalCenterOffset: root.textOffsetX
                    }
                },
                State {
                    name: "topRight"
                    when: root.textAnchor === Alignment.Flag.TopRight
                    AnchorChanges {
                        target: labelItem
                        anchors.bottom: bgPlate.top
                        anchors.left: bgPlate.right
                    }
                    PropertyChanges {
                        labelItem.anchors.bottomMargin: root.textOffsetY
                        labelItem.anchors.leftMargin: root.textOffsetX
                    }
                },
                State {
                    name: "left"
                    when: root.textAnchor === Alignment.Flag.Left
                    AnchorChanges {
                        target: labelItem
                        anchors.right: bgPlate.left
                        anchors.verticalCenter: bgPlate.verticalCenter
                    }
                    PropertyChanges {
                        labelItem.anchors.rightMargin: root.textOffsetX
                        labelItem.anchors.verticalCenterOffset: root.textOffsetY
                    }
                },
                State {
                    name: "center"
                    when: root.textAnchor === Alignment.Flag.Center
                    AnchorChanges {
                        target: labelItem
                        anchors.horizontalCenter: bgPlate.horizontalCenter
                        anchors.verticalCenter: bgPlate.verticalCenter
                    }
                    PropertyChanges {
                        labelItem.anchors.horizontalCenterOffset: root.textOffsetX
                        labelItem.anchors.verticalCenterOffset: root.textOffsetY
                    }
                },
                State {
                    name: "right"
                    when: root.textAnchor === Alignment.Flag.Right
                    AnchorChanges {
                        target: labelItem
                        anchors.left: bgPlate.right
                        anchors.verticalCenter: bgPlate.verticalCenter
                    }
                    PropertyChanges {
                        labelItem.anchors.leftMargin: root.textOffsetX
                        labelItem.anchors.verticalCenterOffset: root.textOffsetY
                    }
                },
                State {
                    name: "bottomLeft"
                    when: root.textAnchor === Alignment.Flag.BottomLeft
                    AnchorChanges {
                        target: labelItem
                        anchors.top: bgPlate.bottom
                        anchors.right: bgPlate.left
                    }
                    PropertyChanges {
                        labelItem.anchors.topMargin: root.textOffsetY
                        labelItem.anchors.rightMargin: root.textOffsetX
                    }
                },
                State {
                    name: "bottom"
                    when: root.textAnchor === Alignment.Flag.Bottom
                    AnchorChanges {
                        target: labelItem
                        anchors.top: bgPlate.bottom
                        anchors.horizontalCenter: bgPlate.horizontalCenter
                    }
                    PropertyChanges {
                        labelItem.anchors.topMargin: root.textOffsetY
                        labelItem.anchors.horizontalCenterOffset: root.textOffsetX
                    }
                },
                State {
                    name: "bottomRight"
                    when: root.textAnchor === Alignment.Flag.BottomRight
                    AnchorChanges {
                        target: labelItem
                        anchors.top: bgPlate.bottom
                        anchors.left: bgPlate.right
                    }
                    PropertyChanges {
                        labelItem.anchors.topMargin: root.textOffsetY
                        labelItem.anchors.leftMargin: root.textOffsetX
                    }
                }
            ]
        }
    }
}
