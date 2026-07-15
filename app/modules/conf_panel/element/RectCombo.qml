import QtQuick
import QtQuick.Controls

import Gentau.Foundation

ComboBox {
    id: control

    property color baseFillColor: Style.grayBlue
    property color hoverColor: Style.lighterGrayBlue
    property color accentColor: Style.lightGreen
    property color textColor: "white"
    property color baseBorderColor: Qt.darker(baseFillColor, 1.5)
    property int baseBorderWidth: 2

    implicitHeight: 35

    font.family: Style.notoSansSC.font.family
    font.pixelSize: 14

    delegate: ItemDelegate {
        id: delegateItem
        width: control.width
        padding: 8

        property color itemTextColor: delegateItem.highlighted ? control.accentColor : control.textColor

        contentItem: Text {
            text: control.textRole ? (Array.isArray(control.model) ? modelData[control.textRole] : model[control.textRole]) : modelData
            color: delegateItem.itemTextColor
            font: control.font
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
        }

        background: Rectangle {
            color: delegateItem.highlighted ? Qt.lighter(control.baseFillColor, 1.2) : "transparent"
            radius: 4
        }

        highlighted: control.highlightedIndex === index
    }

    indicator: Canvas {
        id: canvas
        x: control.width - width - control.rightPadding
        y: control.topPadding + (control.availableHeight - height) / 2
        width: 10
        height: 6
        contextType: "2d"

        Connections {
            target: control
            function onPressedChanged() { canvas.requestPaint(); }
        }

        Connections {
            target: control.popup
            function onVisibleChanged() { canvas.requestPaint(); }
        }

        onPaint: {
            var context = canvas.getContext("2d");
            context.reset();
            context.moveTo(0, 0);
            context.lineTo(width, 0);
            context.lineTo(width / 2, height);
            context.closePath();
            context.fillStyle = (control.pressed || control.popup.visible) ? 
                                control.accentColor : 
                                control.textColor;
            context.fill();
        }
    }

    contentItem: Item {
        width: control.background.width - control.indicator.width - control.spacing - 20
        height: control.background.height

        Text {
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 10

            text: control.displayText
            font: control.font
            color: control.textColor
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }
    }

    background: Rectangle {
        implicitWidth: 120
        implicitHeight: 35

        color: (control.enabled && control.hovered && !control.popup.visible) ? control.hoverColor : Qt.darker(control.baseFillColor, 1.3)

        border.color: (control.pressed || control.popup.visible) ? 
                      control.accentColor : 
                      control.baseBorderColor
        
        border.width: control.baseBorderWidth
        radius: 5

        Behavior on color { ColorAnimation { duration: 150 } }
        Behavior on border.color { ColorAnimation { duration: 150 } }
    }

    popup: Popup {
        y: control.height + 4
        width: control.width
        implicitHeight: contentItem.implicitHeight
        padding: 4

        transformOrigin: Item.TopLeft

        modal: true
        dim: false

        closePolicy: Popup.NoAutoClose | Popup.CloseOnPressOutside | Popup.CloseOnEscape

        scale: typeof scaleFactor !== "undefined" ? scaleFactor : 1.0

        contentItem: ListView {
            clip: true
            implicitHeight: contentHeight
            model: control.popup.visible ? control.delegateModel : null
            currentIndex: control.highlightedIndex

            ScrollIndicator.vertical: ScrollIndicator { }
        }

        background: Rectangle {
            color: Qt.darker(control.baseFillColor, 1.1)
            border.color: control.baseBorderColor
            border.width: control.baseBorderWidth
            radius: 5
        }
    }
}
