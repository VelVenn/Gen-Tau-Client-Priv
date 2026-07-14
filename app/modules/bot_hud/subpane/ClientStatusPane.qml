import QtQuick

import Gentau.CommonElem

Item {
    id: root

    property alias brokerOnline: brokerStatus.isOn
    property alias vt13Online: vt13Status.isOn
    property alias deployVtOnline: deployVtStatus.isOn

    property real scaleFactor: 1.0

    implicitWidth: content.childrenRect.width * scaleFactor
    implicitHeight: content.childrenRect.height * scaleFactor

    Item {
        id: content

        transformOrigin: Item.TopLeft
        scale: root.scaleFactor
        x: -content.childrenRect.x * content.scale
        y: -content.childrenRect.y * content.scale

        Item {
            id: pivot

            width: 0
            height: 0
            x: 0
            y: 0
        }

        Row {
            id: cliRow

            spacing: 5

            IconIndicator {
                id: brokerStatus

                iconIdx: '\ue328'
            }

            IconIndicator {
                id: vt13Status

                iconIdx: '\ue412'
            }

            IconIndicator {
                id: deployVtStatus

                iconIdx: '\uebf4'
            }
        }
    }
}
