import QtQuick

import Gentau.CommonElem

Item {
    id: root

    property alias ctrlState: indCtrl.indicatorState
    property alias powerState: indPower.indicatorState
    property alias vtState: indVT.indicatorState
    property alias armorState: indArmor.indicatorState
    property alias largeShooterState: ind42.indicatorState
    property alias smallShooterState: ind17.indicatorState

    property alias rfidState: indRFID.indicatorState
    property alias uwbState: indUWB.indicatorState
    property alias capacitorState: indCapa.indicatorState
    property alias lightBarState: indLight.indicatorState
    property alias laserState: indLaser.indicatorState

    property real scaleFactor: 1.0

    implicitWidth: content.childrenRect.width * scaleFactor
    implicitHeight: content.childrenRect.height * scaleFactor

    layer.enabled: true
    layer.samples: 4

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
            }

            NotoParalIndicator {
                id: indPower

                labelText: '电源管理'
            }

            NotoParalIndicator {
                id: indVT

                labelText: '图传'
            }

            NotoParalIndicator {
                id: indArmor

                labelText: '装甲'
            }

            NotoParalIndicator {
                id: ind42

                labelText: '42mm'
            }

            NotoParalIndicator {
                id: ind17

                labelText: '17mm'
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
            }

            NotoParalIndicator {
                id: indUWB

                labelText: 'UWB'
            }

            NotoParalIndicator {
                id: indCapa

                labelText: '电容'
            }

            NotoParalIndicator {
                id: indLight

                labelText: '灯条'
            }

            NotoParalIndicator {
                id: indLaser

                labelText: '激光检测'
            }
        }
    }
}
