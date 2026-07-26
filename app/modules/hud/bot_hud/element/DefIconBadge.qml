import QtQuick
import QtQuick.Shapes

import Gentau.Foundation
import Gentau.CommonElem

HorizonGradParalIconBadge {
    id: root

    property real defVal: 0

    QtObject {
        id: param

        property real displayDef: {
            if (root.defVal <= 0) { return 0 }

            return Math.max(1, Math.floor(root.defVal))
        }
    }

    iconIndex: '\ue9e0'
    iconFont.weight: 700
    text: param.displayDef.toString()

    state: defVal > 0 ? 'shown' : 'hidden'

    // 使用 opacity 来驱动可见性，保证淡出动画结束前 Item 仍然可见
    visible: opacity > 0

    borderWidth: 0

    font.family: Style.orbitronFL.font.family
    textOffsetY: 0

    states: [
        State {
            name: "shown"
            PropertyChanges { /*target: root; opacity: 1; scale: 1*/
                root.opacity: 1
                root.scale: 1
            }
        },
        State {
            name: "hidden"
            PropertyChanges { /*target: root; opacity: 0; scale: 0.8*/
                root.opacity: 0
                root.scale: 0.8
            }
        }
    ]

    Behavior on defVal {
        enabled: root.visible

        NumberAnimation {
            duration: 100
            easing.type: Easing.OutQuad
        }
    }

    transitions: Transition {
        NumberAnimation {
            properties: "opacity,scale"
            duration: 100
            easing.type: Easing.OutCubic
        }
    }
}
