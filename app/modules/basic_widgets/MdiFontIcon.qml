import QtQuick

import Gentau.Foundation

Item {
    id: root

    property alias text: icon.text
    property alias font: icon.font

    property color iconColor: 'black'

    property real scaleFactor: 1.0

    implicitWidth: icon.width * scaleFactor
    implicitHeight: icon.height * scaleFactor

    scale: scaleFactor

    Text {
        id: icon

        anchors.centerIn: root

        font.family: Style.mdiIconFL.font.family

        text: '󰋕'

        color: root.iconColor
    }
}
