import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts

import Gentau.Foundation
import Gentau.ConfPanel.Element

ScrollView {
    id: root

    required property var preferences

    component Spilter: Item {
        Layout.fillWidth: true
        Layout.preferredHeight: 10

        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter

            height: 2.5

            color: Qt.darker(Style.grayBlue, 1.5)
        }
    }

    ColumnLayout {
        id: bottomLayout

        width: root.availableWidth

        RowLayout {
            Item {
                Layout.preferredWidth: 3
            }

            Label {
                text: "UI 配置"

                font.family: Style.notoSansSC.font.family
                font.pixelSize: 16

                font.bold: true

                color: 'white'
            }
        }

        Spilter {}

        RectCheckBox {
            id: crossHudChecker

            text: "显示准星与热量环"

            font.pixelSize: 15

            spacing: 20

            checked: root.preferences.showCrosshair

            onToggled: {
                if (root.preferences.showCrosshair !== checked) {
                    root.preferences.showCrosshair = checked;
                }
            }
        }

        RectCheckBox {
            id: spdAndAmmoChecker

            text: "显示射速与弹量"

            font.pixelSize: 15

            spacing: 20

            checked: root.preferences.showSpdAndAmmo

            onToggled: {
                if (root.preferences.showSpdAndAmmo !== checked) {
                    root.preferences.showSpdAndAmmo = checked;
                }
            }
        }

        RectCheckBox {
            id: keyHintChecker

            text: "显示键位提示"

            font.pixelSize: 15

            spacing: 20

            checked: root.preferences.showKeyHint

            onToggled: {
                if (root.preferences.showKeyHint !== checked) {
                    root.preferences.showKeyHint = checked;
                }
            }
        }

        RectCheckBox {
            id: vtUpsideDownChecker

            text: "垂直翻转图传画面"

            font.pixelSize: 15

            spacing: 20

            checked: root.preferences.vtUpsideDown

            onToggled: {
                if (root.preferences.vtUpsideDown !== checked) {
                    root.preferences.vtUpsideDown = checked;
                }
            }
        }

        RowLayout {
            Item {
                Layout.preferredWidth: 3
            }

            Label {
                text: "窗口配置"

                font.family: Style.notoSansSC.font.family
                font.pixelSize: 16

                font.bold: true

                color: 'white'
            }
        }

        Spilter {}

        RectCheckBox {
            id: windowedModeChecker

            text: "窗口化"

            font.pixelSize: 15

            spacing: 20

            checked: root.preferences.windowedMode

            onToggled: {
                if (root.preferences.windowedMode !== checked) {
                    root.preferences.windowedMode = checked;
                }
            }
        }
    }
}
