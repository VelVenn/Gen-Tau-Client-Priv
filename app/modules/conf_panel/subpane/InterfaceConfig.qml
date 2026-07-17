import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts

import Gentau.Foundation
import Gentau.ConfPanel.Element

ScrollView {
    id: root

    property bool initShowCrossChecked: true
    property bool initShowSpdAndAmmoChecked: true
    property bool initShowKeyHintChecked: true

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

            Component.onCompleted: {
                crossHudChecker.checked = root.initShowCrossChecked
            }
        }

        RectCheckBox {
            id: spdAndAmmoChecker

            text: "显示射速与弹量"

            font.pixelSize: 15

            spacing: 20

            Component.onCompleted: {
                spdAndAmmoChecker.checked = root.initShowSpdAndAmmoChecked
            }
        }

        RectCheckBox {
            id: keyHintChecker

            text: "显示键位提示"

            font.pixelSize: 15

            spacing: 20

            Component.onCompleted: {
                keyHintChecker.checked = root.initShowKeyHintChecked
            }
        }
    }
}
