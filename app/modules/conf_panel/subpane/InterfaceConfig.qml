import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts

import Gentau.Foundation
import Gentau.ConfPanel.Element

ScrollView {
    id: root

    required property var preferences;

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

        Item {
            Layout.preferredWidth: 0
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 2

            border.width: 0

            color: Qt.darker(Style.grayBlue, 1.5)
        }

        RectCheckBox {
            id: crossHudChecker

            text: "显示准星与热量环"

            font.pixelSize: 15

            spacing: 20

            checked: root.preferences.showCrosshair

            onToggled: {
                if (root.preferences.showCrosshair !== checked) {
                    root.preferences.showCrosshair = checked
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
                    root.preferences.showSpdAndAmmo = checked
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
                    root.preferences.showKeyHint = checked
                }
            }
        }
    }
}
