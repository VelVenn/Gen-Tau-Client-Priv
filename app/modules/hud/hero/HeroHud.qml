import QtQuick

import Gentau.HeroHud.Subpane

Item {
    id: root

    property real scaleFactor: 1.0

    implicitWidth: 1920
    implicitHeight: 1080

    HeroKeyHintPane {
        id: hintPane

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 15 * root.scaleFactor

        scaleFactor: root.scaleFactor
    }
}
