import QtQuick

import Gentau.Foundation

Item {
    id: root

    property alias iconIdx: icon.text
    property alias font: icon.font

    property color iconColor: 'black'

    property real scaleFactor: 1.0

    implicitWidth: icon.width * scaleFactor
    implicitHeight: icon.height * scaleFactor

    scale: scaleFactor

    Text {
        id: icon

        anchors.centerIn: root

        font.family: Style.materialSharpFL.font.family

        text: '\ue87d'

        color: root.iconColor
    }
}
