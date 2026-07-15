pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects

import Gentau.Foundation
import Gentau.Conf.Element

Popup {
    id: root

    anchors.centerIn: Overlay.overlay

    property real scaleFactor: 1.0

    readonly property real defW: 300
    readonly property real defH: 150

    width: defW * scaleFactor
    height: defH * scaleFactor

    padding: 0

    modal: true
    dim: true
    focus: true

    closePolicy: Popup.NoAutoClose

    enter: Transition {
        ParallelAnimation {
            NumberAnimation {
                property: "opacity"
                from: 0; to: 1
                duration: 200; easing.type: Easing.OutQuad
            }
            NumberAnimation {
                property: "scale"
                from: 0.92; to: 1.0
                duration: 200; easing.type: Easing.OutBack
            }
        }
    }

    exit: Transition {
        ParallelAnimation {
            NumberAnimation {
                property: "opacity"
                from: 1; to: 0
                duration: 150; easing.type: Easing.InQuad
            }
            NumberAnimation {
                property: "scale"
                from: 1.0; to: 0.92
                duration: 150; easing.type: Easing.InQuad
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
            shadowVerticalOffset: 5 * root.scaleFactor
            autoPaddingEnabled: true
        }
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
                anchors.margins: 20
                spacing: 10

                Label {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    
                    text: "确定要退出程序吗？"
                    font.family: Style.notoSansSC.font.family
                    font.pixelSize: 22
                    color: "white"
                    
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                RowLayout {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 30
                    spacing: 20

                    RectButton {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        
                        text: "是"
                        textColor: Style.lightFire
                        baseBorderColor: Style.lightFire
                        baseFillColor: Qt.lighter(Style.grayBlue, 1.1)
                        
                        font.pixelSize: 18
                        font.bold: true
                        
                        onClicked: {
                            Qt.quit()
                        }
                    }

                    RectButton {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        
                        text: "否"
                        textColor: Style.lightGreen
                        baseBorderColor: Style.lightGreen
                        baseFillColor: Qt.lighter(Style.grayBlue, 1.1)
                        
                        font.pixelSize: 18
                        font.bold: true
                        
                        onClicked: {
                            root.close()
                        }
                    }
                }
            }
        }
    }
}
