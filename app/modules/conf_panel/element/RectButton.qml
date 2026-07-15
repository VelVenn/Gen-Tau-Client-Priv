import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Controls.Material.impl as MaterialImpl

import Gentau.Foundation

Button {
    id: control
    
    property color baseFillColor: Style.grayBlue
    property color hoverColor: Style.lighterGrayBlue
    property color textColor: "white"
    property color baseBorderColor: Qt.darker(baseFillColor, 1.5)
    property int baseBorderWidth: 2

    implicitHeight: 35
    implicitWidth: 120

    font.family: Style.notoSansSC.font.family
    font.pixelSize: 14
    
    hoverEnabled: true
    MouseArea {
        anchors.fill: parent
        cursorShape: control.enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
        acceptedButtons: Qt.NoButton 
    }

    contentItem: Text {
        text: control.text
        font: control.font
        color: control.textColor
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        id: bg
        color: (control.enabled && control.hovered) ? control.hoverColor : Qt.darker(control.baseFillColor, 1.3)
        border.color: control.baseBorderColor
        border.width: control.baseBorderWidth
        radius: 5

        Behavior on color { ColorAnimation { duration: 250 } }

        property bool isRipplePressed: false
        
        Timer {
            id: rippleHoldTimer
            interval: 150
            onTriggered: {
                if (!control.down) {
                    bg.isRipplePressed = false;
                }
            }
        }

        Connections {
            target: control
            function onDownChanged() {
                if (control.down) {
                    bg.isRipplePressed = true;
                    rippleHoldTimer.restart();
                } else {
                    if (!rippleHoldTimer.running) {
                        bg.isRipplePressed = false;
                    }
                }
            }
        }

        MaterialImpl.Ripple {
            anchors.fill: parent
            anchors.margins: 2
            clip: true
            clipRadius: 3
            
            anchor: control 
            
            pressed: bg.isRipplePressed
            active: bg.isRipplePressed
            color: Qt.rgba(1, 1, 1, 0.15)
        }
    }
}
