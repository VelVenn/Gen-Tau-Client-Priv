import QtQuick

import Gentau.Foundation
import Gentau.BasicWidgets
import Gentau.CommonElem

Item {
    id: root

    property int botIdx: 1
    property real botHp: 250

    property bool leftSlant: true

    property real scaleFactor: 1.0

    property color bgColor: Qt.alpha(Style.grayColor, 0.6)

    implicitWidth: content.childrenRect.width * scaleFactor
    implicitHeight: content.childrenRect.height * scaleFactor

    QtObject {
        id: param

        property int type: BotMeta.toBotType(root.botIdx)
        property int camp: BotMeta.toBotCamp(root.botIdx)
        property int realIdx: BotMeta.toRealBotIdx(root.botIdx)

        property string name: BotMeta.toBotName(root.botIdx)

        property color baseColor: camp === BotMeta.BotCamp.RED ? Style.redColor : Style.blueColor

        property real displayHp: {
            if (root.botHp <= 0) { return 0 }

            return Math.max(1, Math.floor(root.botHp))
        }

        Behavior on baseColor {
            ColorAnimation {
                duration: 250
                easing.type: Easing.OutQuad
            }
        }
    }

    Behavior on botHp {
        enabled: root.visible

        NumberAnimation {
            duration: 200
            easing.type: Easing.OutQuad
        }
    }

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

        ParalBadge {
            id: nameBadge

            text: param.name

            anchors.top: pivot.top

            font.family: Style.notoSansSC.font.family
            font.pixelSize: 13

            height: 15
            width: 55

            textOffsetY: 1

            slantWidth: root.leftSlant ? -6 : 6

            textColor: 'white'

            borderColor: param.baseColor
            borderWidth: 2
            bgColor: root.bgColor
        }

        ParalBadge {
            id: idxBadge

            text: param.realIdx.toString()

            anchors.verticalCenter: nameBadge.verticalCenter
            anchors.right: root.leftSlant ? nameBadge.left : undefined
            anchors.left: root.leftSlant ? undefined : nameBadge.right

            anchors.rightMargin: root.leftSlant ? -1 : 0
            anchors.leftMargin: root.leftSlant ? 0 : -1

            font.family: Style.orbitronFL.font.family
            font.pixelSize: 13

            height: 15
            width: 30

            slantWidth: root.leftSlant ? -6 : 6

            textColor: 'white'
            textOffsetY: 1
            textOffsetX: 1

            borderColor: param.baseColor
            borderWidth: 2
            bgColor: root.bgColor
        }

        HorizonGradParalIconBadge {
            id: hpBadge

            anchors.top: nameBadge.bottom
            anchors.left: root.leftSlant ? idxBadge.left : undefined
            anchors.right: root.leftSlant ? undefined : idxBadge.right

            anchors.topMargin: 5

            anchors.leftMargin: root.leftSlant ? 8.5 : 0
            anchors.rightMargin: root.leftSlant ? 0 : 6.5

            baseColor: param.baseColor
            lighterOnLeft: !root.leftSlant

            enableColorAnim: false

            width: {
                var baseW = nameBadge.width + idxBadge.width + 2

                return root.leftSlant ? baseW + idxBadge.anchors.rightMargin : baseW + idxBadge.anchors.leftMargin
            }

            height: 18

            slantWidth: root.leftSlant ? -7.5 : 7.5

            text: param.displayHp
            textOffsetY: 0
            font.family: Style.orbitronFL.font.family
            font.pixelSize: 15

            iconOnLeft: root.leftSlant
            iconIndex: '\uf2c3'
        }
    }
}
