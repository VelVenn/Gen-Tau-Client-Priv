import QtQuick
import QtQuick.Shapes
import QtQuick.Effects

import Gentau.Foundation

Item {
    id: root

    // --- 状态 ---
    property bool isOn: false

    // --- 闪烁 ---
    property bool blinkEnabled: false      // On 状态下是否闪烁
    property int blinkInterval: 600        // 闪烁周期 (ms)

    // --- 指示灯颜色 ---
    property color onColor: Qt.alpha("#22DD44", 1.0)     // On 状态颜色
    property color offColor: Qt.alpha("#3A3A42", 0.6)    // Off 状态颜色

    // --- Paral 几何属性 ---
    property real lampWidth: 12
    property real lampHeight: 18
    property real slantWidth: 5.0
    property real borderRadius: 1.5

    // --- 边框（深色描边） ---
    property color borderColor: Qt.rgba(0.08, 0.08, 0.10, 0.85)
    property real borderWidth: 1.2

    // --- Glow 光晕 ---
    property real glowRadius: 8.0          // 光晕扩散半径
    property real glowSpread: 0.4          // 光晕集中度 (0~1)
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
    property real textOffsetX: 3.0                  // 锚点偏移 X
    property real textOffsetY: 1.0                  // 锚点偏移 Y

    property real scaleFactor: 1.0

    implicitWidth: content.childrenRect.width * scaleFactor
    implicitHeight: content.childrenRect.height * scaleFactor

    QtObject {
        id: param

        // 闪烁可见性标志（On + blinkEnabled 时交替翻转）
        property bool blinkVisible: true

        // 状态切换瞬间跳过 Behavior 动画
        property bool skipAnimation: false

        // 当前有效颜色：叠加闪烁逻辑
        readonly property color activeColor: {
            if (!root.isOn)
                return root.offColor;
            if (root.blinkEnabled && !blinkVisible)
                return root.offColor;
            return root.onColor;
        }

        // 是否应该发光 —— 只在灯"点亮"帧发光
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
                param.blinkVisible = false;  // 立即进入暗相，让用户马上看到闪烁
            else
                param.blinkVisible = true;   // 停止闪烁时归位亮相
            param.skipAnimation = false;
        }
    }

    Item {
        id: content

        transformOrigin: Item.TopLeft
        scale: root.scaleFactor
        x: -content.childrenRect.x * content.scale
        y: -content.childrenRect.y * content.scale

        // --- 光晕层 (Glow) ---
        Item {
            id: glowSourceContainer
            anchors.centerIn: lamp
            width: lamp.width + root.glowRadius * 2
            height: lamp.height + root.glowRadius * 2
            visible: false  // 仅作为 MultiEffect 源

            Paral {
                id: glowSource
                anchors.centerIn: parent

                width: lamp.width
                height: lamp.height

                slantWidth: root.slantWidth
                borderRadius: root.borderRadius

                bgColor: param.activeColor
                borderWidth: 0
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

        Paral {
            id: lamp
            x: 0
            y: 0
            width: root.lampWidth
            height: root.lampHeight

            slantWidth: root.slantWidth
            borderRadius: root.borderRadius

            bgColor: param.activeColor
            borderColor: root.borderColor
            borderWidth: root.borderWidth

            // 灯色切换平滑过渡
            Behavior on bgColor {
                enabled: root.blinkEnabled && !param.skipAnimation && root.visible
                ColorAnimation {
                    duration: 100
                    easing.type: Easing.OutQuad
                }
            }
        }

        Paral {
            id: highlight

            anchors.top: lamp.top
            anchors.left: lamp.left
            anchors.right: lamp.right

            anchors.topMargin: root.borderWidth
            anchors.leftMargin: root.borderWidth
            anchors.rightMargin: root.borderWidth

            height: lamp.height * 0.45

            slantWidth: root.slantWidth * (height / lamp.height)

            borderRadius: Math.max(0.5, root.borderRadius - root.borderWidth)
            radiusBL: 0
            radiusBR: 0

            bgColor: "transparent"
            borderWidth: 0

            gradientPreset: GradientPreset.LighterOnTop
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

            // ---- 九宫格锚点定位（相对 lamp） ----
            states: [
                // ---------- 上排 ----------
                State {
                    name: "topLeft"
                    when: root.textAnchor === Alignment.Flag.TopLeft
                    AnchorChanges {
                        target: labelItem
                        anchors.bottom: lamp.top
                        anchors.right: lamp.left
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
                        anchors.bottom: lamp.top
                        anchors.horizontalCenter: lamp.horizontalCenter
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
                        anchors.bottom: lamp.top
                        anchors.left: lamp.right
                    }
                    PropertyChanges {
                        labelItem.anchors.bottomMargin: root.textOffsetY
                        labelItem.anchors.leftMargin: root.textOffsetX
                    }
                },

                // ---------- 中排 ----------
                State {
                    name: "left"
                    when: root.textAnchor === Alignment.Flag.Left
                    AnchorChanges {
                        target: labelItem
                        anchors.right: lamp.left
                        anchors.verticalCenter: lamp.verticalCenter
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
                        anchors.horizontalCenter: lamp.horizontalCenter
                        anchors.verticalCenter: lamp.verticalCenter
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
                        anchors.left: lamp.right
                        anchors.verticalCenter: lamp.verticalCenter
                    }
                    PropertyChanges {
                        labelItem.anchors.leftMargin: root.textOffsetX
                        labelItem.anchors.verticalCenterOffset: root.textOffsetY
                    }
                },

                // ---------- 下排 ----------
                State {
                    name: "bottomLeft"
                    when: root.textAnchor === Alignment.Flag.BottomLeft
                    AnchorChanges {
                        target: labelItem
                        anchors.top: lamp.bottom
                        anchors.right: lamp.left
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
                        anchors.top: lamp.bottom
                        anchors.horizontalCenter: lamp.horizontalCenter
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
                        anchors.top: lamp.bottom
                        anchors.left: lamp.right
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
