import QtQuick

import Gentau.CommonElem

import Gentau.Bot.Common
import Gentau.Message

Item {
    id: root

    required property BotCommonStatus commonStatus
    
    readonly property robotModuleStatus modStatus: commonStatus.modStatus

    readonly property int ctrlState: modStatus.mainController
    readonly property int powerState: modStatus.powerManager
    readonly property int vtState: modStatus.videoTransmission
    readonly property int armorState: modStatus.armor
    readonly property int largeShooterState: modStatus.bigShooter
    readonly property int smallShooterState: modStatus.smallShooter

    readonly property int rfidState: modStatus.rfid
    readonly property int uwbState: modStatus.uwb
    readonly property int capacitorState: modStatus.capacitor
    readonly property int lightBarState: modStatus.lightStrip
    readonly property int laserState: modStatus.laserDetectionModule

    property real scaleFactor: 1.0

    implicitWidth: content.childrenRect.width * scaleFactor
    implicitHeight: content.childrenRect.height * scaleFactor

    // layer.enabled: true
    // layer.samples: 4

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
            id: upperRow

            NotoParalIndicator {
                id: indCtrl

                labelText: '主控'
                indicatorState: root.ctrlState
            }

            NotoParalIndicator {
                id: indPower

                labelText: '电源管理'
                indicatorState: root.powerState
            }

            NotoParalIndicator {
                id: indVT

                labelText: '图传'
                indicatorState: root.vtState
            }

            NotoParalIndicator {
                id: indArmor

                labelText: '装甲'
                indicatorState: root.armorState
            }

            NotoParalIndicator {
                id: ind42

                labelText: '大弹丸'
                indicatorState: root.largeShooterState
            }

            NotoParalIndicator {
                id: ind17

                labelText: '小弹丸'
                indicatorState: root.smallShooterState
            }
        }

        Row {
            id: lowerRow

            anchors.top: upperRow.bottom
            anchors.left: upperRow.left

            anchors.topMargin: -5

            spacing: 15.5

            NotoParalIndicator {
                id: indRFID

                labelText: 'RFID'
                indicatorState: root.rfidState
            }

            NotoParalIndicator {
                id: indUWB

                labelText: 'UWB'
                indicatorState: root.uwbState
            }

            NotoParalIndicator {
                id: indCapa

                labelText: '电容'
                indicatorState: root.capacitorState
            }

            NotoParalIndicator {
                id: indLight

                labelText: '灯条'
                indicatorState: root.lightBarState
            }

            NotoParalIndicator {
                id: indLaser

                labelText: '激光检测'
                indicatorState: root.laserState
            }
        }
    }
}
