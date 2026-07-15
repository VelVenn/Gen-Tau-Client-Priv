import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Controls.Material.impl as MaterialImpl

import Gentau.Foundation
import Gentau.BasicWidgets

CheckBox {
    id: root
    
    property color baseFillColor: Style.grayBlue
    property color hoverColor: Style.lighterGrayBlue
    property color textColor: "white"
    property color baseBorderColor: Qt.darker(baseFillColor, 1.5)
    property int baseBorderWidth: 2

    property color iconColor: Style.lightGreen
    property string checkedIcon: "\ue5ca"
    property string partialCheckedIcon: "\uf88a"
    
    property int indicatorSize: 22

    font.family: Style.notoSansSC.font.family
    font.pixelSize: 14
    
    spacing: 10
    
    hoverEnabled: true
    
    MouseArea {
        anchors.fill: parent
        cursorShape: root.enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
        acceptedButtons: Qt.NoButton 
    }

    indicator: Rectangle {
        id: indRect

        implicitWidth: root.indicatorSize
        implicitHeight: root.indicatorSize
        x: root.leftPadding
        y: parent.height / 2 - height / 2
        
        radius: 4
        border.color: root.baseBorderColor
        border.width: root.baseBorderWidth
        color: (root.enabled && root.hovered) ? root.hoverColor : Qt.darker(root.baseFillColor, 1.3)

        Behavior on color { ColorAnimation { duration: 250 } }

        FontIcon {
            anchors.centerIn: parent
            iconIdx: root.checkState === Qt.PartiallyChecked ? root.partialCheckedIcon : root.checkedIcon
            iconColor: root.iconColor
            visible: root.checkState === Qt.Checked || root.checkState === Qt.PartiallyChecked
            font.pixelSize: root.indicatorSize - 4
        }
        
        property bool isRipplePressed: false
        
        Timer {
            id: rippleHoldTimer
            interval: 150
            onTriggered: {
                if (!root.down) {
                    root.indicator.isRipplePressed = false;
                }
            }
        }

        Connections {
            target: root
            function onDownChanged() {
                if (root.down) {
                    root.indicator.isRipplePressed = true;
                    rippleHoldTimer.restart();
                } else {
                    if (!rippleHoldTimer.running) {
                        root.indicator.isRipplePressed = false;
                    }
                }
            }
        }

        MaterialImpl.Ripple {
            anchors.fill: parent
            anchors.margins: 1
            clip: true
            clipRadius: 3
            
            anchor: root.indicator
            
            pressed: indRect.isRipplePressed
            active: indRect.isRipplePressed
            color: Qt.rgba(1, 1, 1, 0.15)
        }
    }

    contentItem: Text {
        text: root.text
        font: root.font
        color: root.textColor
        verticalAlignment: Text.AlignVCenter
        leftPadding: root.indicator.width + root.spacing
    }
}
