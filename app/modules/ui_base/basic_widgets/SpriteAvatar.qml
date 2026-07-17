import QtQuick
import QtQuick.Effects

import Gentau.Foundation

Item {
    id: root

    readonly property real _baseX: {
        switch (root.alignment) {
        case Alignment.Flag.TopLeft:
        case Alignment.Flag.Left:
        case Alignment.Flag.BottomLeft:
            return 0
        case Alignment.Flag.TopRight:
        case Alignment.Flag.Right:
        case Alignment.Flag.BottomRight:
            return imgContainer.width - avatarImg.width
        default: // Center, Top, Bottom
            return (imgContainer.width - avatarImg.width) / 2
        }
    }

    readonly property real _baseY: {
        switch (root.alignment) {
        case Alignment.Flag.TopLeft:
        case Alignment.Flag.Top:
        case Alignment.Flag.TopRight:
            return 0
        case Alignment.Flag.BottomLeft:
        case Alignment.Flag.Bottom:
        case Alignment.Flag.BottomRight:
            return imgContainer.height - avatarImg.height
        default: // Center, Left, Right
            return (imgContainer.height - avatarImg.height) / 2
        }
    }

    property int alignment: Alignment.Flag.Center

    property real roiX: 0
    property real roiY: 0
    property real roiW: 120   // 裁取区域的宽
    property real roiH: 120   // 裁取区域的高

    property alias imgSrc: avatarImg.source
    property alias imgScale: avatarImg.scale
    property real imgOffsetX: 0
    property real imgOffsetY: 0

    property alias radius: circleMask.radius
    property alias radiusTL: circleMask.topLeftRadius
    property alias radiusBL: circleMask.bottomLeftRadius
    property alias radiusTR: circleMask.topRightRadius
    property alias radiusBR: circleMask.bottomRightRadius

    property alias border: borderMask.border

    property alias bgColor: bgFill.color

    implicitWidth: 120
    implicitHeight: 120

    Rectangle{
        id: bgFill

        anchors.fill: parent

        radius: root.radius
        topLeftRadius: root.radiusTL
        topRightRadius: root.radiusTR
        bottomLeftRadius: root.radiusBL
        bottomRightRadius: root.radiusBR

        border.width: 0
        border.color: 'transparent'

        color: 'transparent'
    }

    Item {
        id: imgContainer
        anchors.fill: parent
        visible: false
        Image {
            id: avatarImg
            source: 'images/Hero.png'

            sourceClipRect: Qt.rect(root.roiX, root.roiY,
                                    root.roiW, root.roiH)

            x: root._baseX + root.imgOffsetX
            y: root._baseY + root.imgOffsetY

            scale: root.imgScale
            transformOrigin: Item.Center

            asynchronous: true
            antialiasing: true
        }
    }

    Rectangle {
        id: circleMask
        anchors.fill: parent

        radius: width
        border.width: 0

        visible: false

        layer.enabled: true
    }

    MultiEffect {
        id: maskEffect

        anchors.fill: parent

        source: imgContainer
        smooth: true

        maskEnabled: true
        maskSource: circleMask
    }

    Rectangle {
        id: borderMask
        anchors.centerIn: parent

        width: root.width + border.width
        height: root.height + border.width

        color: 'transparent'

        radius: root.radius
        topLeftRadius: root.radiusTL
        topRightRadius: root.radiusTR
        bottomLeftRadius: root.radiusBL
        bottomRightRadius: root.radiusBR

        border.width: 2
        border.color: "Red"
    }
}
