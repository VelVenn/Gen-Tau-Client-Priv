import QtQuick

import Gentau.CommonElem
import Gentau.Service.Conn

Item {
    id: root

    required property ConnService connService

    readonly property bool brokerOnline: root.connService.clientId !== ""
    readonly property bool vt13Online: connService.vt13Online
    readonly property bool deployVtOnline: connService.deployVtOnline

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

                isOn: root.brokerOnline
            }

            IconIndicator {
                id: vt13Status

                iconIdx: '\ue412'

                isOn: root.vt13Online
            }

            IconIndicator {
                id: deployVtStatus

                iconIdx: '\uebf4'

                isOn: root.deployVtOnline
            }
        }
    }
}
