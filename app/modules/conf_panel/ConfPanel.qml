import QtQuick

import QtQuick.Controls

import QtQuick.Layouts
import QtQuick.Effects

import Gentau.Foundation
import Gentau.Conf.Element
import Gentau.Conf.Subpane

Popup {
    id: root
    // --- 定位 ---
    anchors.centerIn: Overlay.overlay   // 自动在 Overlay 层居中

    property real scaleFactor: 1.0
    
    property int initialTabIndex: 0

    readonly property int currentTabIndex: navBar.currentIndex

    Component.onCompleted: {
        if (root.initialTabIndex !== 0) {
            Qt.callLater(() => { navBar.currentIndex = root.initialTabIndex })
        }
    }

    readonly property real defW: 640
    readonly property real defH: 880

    width: defW * scaleFactor
    height: defH * scaleFactor

    padding: 0

    // --- 交互行为 ---
    modal: true        // 背后的 HUD 不响应鼠标
    dim: true          // 自动给背景加一层半透明遮罩
    focus: true        // Popup 打开时自动获取焦点（使键盘事件进来）

    closePolicy: Popup.CloseOnPressOutside | Popup.NoAutoClose

    enter: Transition {
        ParallelAnimation {
            NumberAnimation {
                property: "opacity"
                from: 0
                to: 1
                duration: 200
                easing.type: Easing.OutQuad
            }
            NumberAnimation {
                property: "scale"
                from: 0.92
                to: 1.0
                duration: 200
                easing.type: Easing.OutBack
            }
        }
    }

    exit: Transition {
        ParallelAnimation {
            NumberAnimation {
                property: "opacity"
                from: 1
                to: 0
                duration: 150
                easing.type: Easing.InQuad
            }
            NumberAnimation {
                property: "scale"
                from: 1.0
                to: 0.92
                duration: 150
                easing.type: Easing.InQuad
            }
        }
    }

    QtObject {
        id: param

        property int confPage
    }

    contentItem: Item {
        id: wrapper

        Item {
            id: content

            width: root.defW
            height: root.defH

            scale: root.scaleFactor
            transformOrigin: Item.TopLeft

            ColumnLayout {
                anchors.fill: parent

                anchors.margins: 15

                Label {
                    text: "设置面板"

                    font.family: Style.notoSansSC.font.family
                    font.pixelSize: 30

                    color: 'white'

                    Layout.alignment: Qt.AlignHCenter
                }

                Item {
                    Layout.preferredHeight: 5
                }

                SpringSlideTabBar {
                    id: navBar

                    Layout.alignment: Qt.AlignHCenter
                    Layout.fillWidth: true

                    contentAlignment: Qt.AlignLeft

                    Repeater {
                        model: ['机器人', '界面', '通用']

                        SlideNavTab {
                            text: modelData

                            width: 100
                        }
                    }
                }

                Item {
                    Layout.preferredHeight: 5
                }

                StackLayout {
                    id: confPages

                    // Layout.fillHeight: true
                    Layout.fillWidth: true
                    
                    currentIndex: navBar.currentIndex

                    BotConfig {
                        id: botConfig
                    }

                    InterfaceConfig {
                        id: interfaceConfig
                    }

                    // Item {
                    //     Label {
                    //         text: "施工中      (*^_^*)"

                    //         font.family: Style.notoSansSC.font.family
                    //         font.pixelSize: 40

                    //         color: 'white'

                    //         anchors.centerIn: parent
                    //     }
                    // }

                    GeneralConfig {
                        id: generalConfig

                        popupScaler: root.scaleFactor
                    }
                }

                Item {
                    Layout.fillHeight: true
                }
            }
        }
    }

    background: Rectangle {
        color: Style.grayBlue
        border.color: Qt.lighter(Style.grayBlue, 1.5)
        border.width: Math.max(1, Math.round(3 * root.scaleFactor))
        radius: 10 * root.scaleFactor

        layer.enabled: true
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowColor: Qt.rgba(0, 0, 0, 0.8)

            shadowBlur: 0.8
            // shadowScale: 1.05

            shadowVerticalOffset: 5 * root.scaleFactor

            autoPaddingEnabled: true
        }
    }
}
