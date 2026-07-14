pragma ComponentBehavior: Bound

import QtQuick

import Gentau.Foundation
import Gentau.BotHud.Element

Item {
    id: root

    property int ourCamp: BotMeta.BotCamp.RED

    property var botHpData: null

    property real scaleFactor: 1.0

    implicitWidth: content.childrenRect.width * scaleFactor
    implicitHeight: content.childrenRect.height * scaleFactor

    QtObject {
        id: param

        readonly property var redBotIdx: [1, 2, 3, 4, 7]
        readonly property var blueBotIdx: [101, 102, 103, 104, 107]

        readonly property var ourBotIdx: root.ourCamp === BotMeta.BotCamp.RED
                                         ? redBotIdx : blueBotIdx

        readonly property var theirBotIdx: root.ourCamp === BotMeta.BotCamp.RED
                                           ? blueBotIdx : redBotIdx

        property var mockHpData:
            [250, 250, 250, 250, 250,
            250, 250, 250, 250, 250]

        readonly property var displayHpData: root.botHpData ?? mockHpData
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

        Row {
            id: ourTeamBotsStat

            spacing: 5

            anchors.verticalCenter: centerPadding.verticalCenter
            anchors.right: centerPadding.left

            Repeater {
                model: 5

                TeamBotStatusBadge {
                    required property int index

                    botIdx:      param.ourBotIdx[4 - index]
                    botHp:       param.displayHpData[4 - index]
                    leftSlant:   true
                }
            }
        }

        Rectangle {
            id: centerPadding

            height: 1
            width: 450

            color: 'transparent'
            border.color: 'transparent'

            anchors.centerIn: pivot
        }

        Row {
            id: theirTeamBotsStat

            spacing: 7

            anchors.verticalCenter: centerPadding.verticalCenter
            anchors.left: centerPadding.right

            Repeater {
                model: 5

                TeamBotStatusBadge {
                    required property int index

                    botIdx:      param.theirBotIdx[index]
                    botHp:       param.displayHpData[5 + index]
                    leftSlant:   false
                }
            }
        }
    }
}
