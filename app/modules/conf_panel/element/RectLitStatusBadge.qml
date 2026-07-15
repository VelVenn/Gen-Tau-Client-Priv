import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Gentau.Foundation

GroupBox {
    id: root

    property int contentVAlign: Qt.AlignVCenter

    property alias hintText: hintLabel.text
    property alias statusText: statusLabel.text

    property alias hintTextColor: hintLabel.color
    property alias statusTextColor: statusLabel.color

    property alias hintTextFont: hintLabel.font
    property alias statusTextFont: statusLabel.font

    property int rightFillWidth: 10

    background: Rectangle {
        color: 'transparent'
        border.color: Qt.darker(Style.grayBlue, 1.5)
        border.width: 2
        radius: 5
    }

    contentItem: Item {
        implicitHeight: row.implicitHeight
        implicitWidth: row.implicitWidth

        RowLayout {
            id: row
            width: parent.width

            anchors.verticalCenter: root.contentVAlign === Qt.AlignVCenter ? parent.verticalCenter : undefined
            anchors.top: root.contentVAlign === Qt.AlignTop ? parent.top : undefined
            anchors.bottom: root.contentVAlign === Qt.AlignBottom ? parent.bottom : undefined

            Label {
                id: hintLabel

                text: "Hint："

                font.family: Style.notoSansSC.font.family
                font.pixelSize: 13

                color: "white"
            }

            Item {
                Layout.fillWidth: true
            }

            Label {
                id: statusLabel

                text: "Status"

                font.family: Style.orbitronFL.font.family
                font.pixelSize: 15
                font.bold: true

                color: "white"
            }

            Item {
                id: rightFill

                Layout.preferredWidth: root.rightFillWidth
            }
        }
    }
}
