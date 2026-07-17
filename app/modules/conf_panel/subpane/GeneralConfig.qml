import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts

import Gentau.Foundation
import Gentau.ConfPanel.Element

ScrollView {
    id: root

    property real popupScaler: 1.0

    ColumnLayout {
        id: bottomLayout

        width: root.availableWidth

        RowLayout {
            id: exitCard

            Layout.fillWidth: true

            Item {
                Layout.fillWidth: true
                Layout.preferredWidth: 2
            }

            RectButton {
                id: exitButton

                Layout.fillWidth: true
                Layout.preferredWidth: 3

                Layout.preferredHeight: 50

                baseBorderColor: Style.lightFire

                text: "退        出        程        序"
                textColor: Style.lightFire

                baseFillColor: Qt.lighter(Style.grayBlue, 1.1)

                font.pixelSize: 16
                font.bold: true

                onClicked: {
                    if (!exitPopup.opened) {
                        exitPopup.open()
                    }
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.preferredWidth: 2
            }
        }
    }

    ExitProgramPopup {
        id: exitPopup

        scaleFactor: root.popupScaler
    }
}
