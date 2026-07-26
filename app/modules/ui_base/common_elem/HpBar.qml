import QtQuick
import QtQuick.Effects

import Gentau.Foundation
import Gentau.BasicWidgets

ParalProgressBar {
    id: root

    property bool isHealing: false
    property bool isInvincible: false

    property bool displayText: true

    property real targetHp: 0

    property real lowHpThreshold: 0.1

    property color baseColor: 'red'
    property color invincibleColor: 'yellow'

    property int displayHp: 0

    function updateDisplayHp() {
        if (root.value <= 0) { displayHp = 0 }
        else if (root.value >= root.maxValue) { displayHp = root.maxValue }
        else { displayHp = Math.max(1, Math.floor(root.value)) }
    }

    onValueChanged: updateDisplayHp()
    Component.onCompleted: updateDisplayHp()

    QtObject {
        id: param

        property bool maxValJustChanged: false
        property color displayInvColor
    }

    width: 500

    onTargetHpChanged: {
        isHealing = targetHp > value;
        value = targetHp;
    }

    onMaxValueChanged: {
        param.maxValJustChanged = true
        value = targetHp
        updateDisplayHp()
        param.maxValJustChanged = false
    }

    font.family: Style.orbitronFL.font.family
    font.pixelSize: 20

    fillColor: (value / maxValue) < lowHpThreshold ? Qt.darker(baseColor, 1.7) : baseColor
    bgColor: Qt.alpha('gray', 0.6)

    borderColor: isInvincible ? invincibleColor : Qt.lighter(bgColor, 1.2)
    borderWidth: 3

    textOffsetY: 2
    text: displayText ? displayHp.toString() : ''

    Behavior on value {
        id: hpValAnim

        enabled: root.visible && !param.maxValJustChanged

        NumberAnimation {
            duration: root.isHealing ? 700 : 200
            easing.type: Easing.OutQuad
        }
    }

    Behavior on fillColor {
        enabled: root.visible

        ColorAnimation {
            duration: 250
            easing.type: Easing.InOutQuad
        }
    }

    Behavior on borderColor {
        enabled: root.visible

        ColorAnimation {
            duration: 250
            easing.type: Easing.InOutQuad
        }
    }

    // SequentialAnimation {
    //     running: root.isInvincible && root.visible
    //     loops: Animation.Infinite

    //     ColorAnimation {
    //         target: param
    //         property: "displayInvColor"
    //         from: root.invincibleColor
    //         to: Qt.lighter(root.bgColor, 1.2)
    //         duration: 600
    //         easing.type: Easing.Linear
    //     }

    //     ColorAnimation {
    //         target: param
    //         property: "displayInvColor"
    //         from: Qt.lighter(root.bgColor, 1.2)
    //         to: root.invincibleColor
    //         duration: 600
    //         easing.type: Easing.Linear
    //     }
    // }

    layer.enabled: true
    layer.effect: MultiEffect{
        shadowEnabled: true
        shadowColor: Qt.rgba(0,0,0,0.6)

        shadowBlur: 0.8

        shadowHorizontalOffset: 0
        shadowVerticalOffset: 5

        // 这个属性会让光晕自由扩散，并且不会被矩形边界像切豆腐一样切成硬直角死边！
        autoPaddingEnabled: true
    }
}

