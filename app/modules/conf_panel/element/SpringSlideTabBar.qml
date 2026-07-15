import QtQuick
import QtQuick.Controls

import Gentau.Foundation

TabBar {
    id: root
    objectName: "customSlideTabBar"

    property int contentAlignment: Qt.AlignLeft
    property bool tabFillWidth: false

    implicitWidth: {
        var w = 0;
        for (var i = 0; i < count; i++) {
            var item = itemAt(i);
            if (item) w += item.implicitWidth;
        }
        if (count > 0) w += (count - 1) * spacing;
        return Math.max(implicitBackgroundWidth, w) + leftPadding + rightPadding;
    }

    property real sliderTargetX: root.currentItem ? root.currentItem.x : 0
    property real sliderTargetWidth: root.currentItem ? root.currentItem.width : 0

    property alias bgColor: bg.color
    property alias bgBorder: bg.border
    property alias bgRad: bg.radius

    property alias slideBarColor: slideBar.color
    property alias slideBarBorder: slideBar.border
    property alias slideBarRad: slideBar.radius

    Behavior on sliderTargetX {
        enabled: root.visible && param.isInit

        SpringAnimation {
            spring: 3.5
            damping: 0.3
        }
    }

    Behavior on sliderTargetWidth {
        enabled: root.visible && param.isInit

        SpringAnimation {
            spring: 3.5
            damping: 0.3
        }
    }

    QtObject {
        id: param
        property bool isInit: false
    }

    Timer {
        id: initTimer
        interval: 50
        onTriggered: param.isInit = true
    }

    onVisibleChanged: {
        if (visible) {
            initTimer.restart()
        } else {
            param.isInit = false
            initTimer.stop()
        }
    }

    Component.onCompleted: {
        if (visible) initTimer.start()
    }

    spacing: 10

    topPadding: 8
    bottomPadding: 8
    leftPadding: 10
    rightPadding: 10

    background: Rectangle {
        id: bg

        color: Qt.darker(Style.grayBlue, 1.2)
        border.color: Style.grayColor
        border.width: 3
        radius: 8
        clip: true

        Rectangle {
            id: slideBar

            x: root.contentItem.x + root.sliderTargetX - root.contentItem.contentX
            y: root.contentItem.y + (root.currentItem ? root.currentItem.y : 0)
            width: root.sliderTargetWidth
            height: root.currentItem ? root.currentItem.height : 0

            radius: 8
            color: Qt.alpha(Qt.lighter(Style.grayBlue, 1.5), 0.6)
            border.color: Qt.alpha("white", 0.25)
            border.width: 1
            visible: root.currentItem !== null
        }
    }

    contentItem: ListView {
        id: listView
        model: root.contentModel
        currentIndex: root.currentIndex

        spacing: root.spacing
        orientation: ListView.Horizontal
        boundsBehavior: Flickable.StopAtBounds
        flickableDirection: Flickable.AutoFlickIfNeeded
        snapMode: ListView.SnapToItem

        highlightMoveDuration: 300
        highlightMoveVelocity: -1

        width: Math.min(contentWidth, root.availableWidth)
        height: root.availableHeight

        x: {
            if (root.contentAlignment === Qt.AlignHCenter)
                return root.leftPadding + (root.availableWidth - width) / 2;
            else if (root.contentAlignment === Qt.AlignRight)
                return root.width - root.rightPadding - width;
            else
                return root.leftPadding;
        }
        y: root.topPadding

        clip: true
    }
}
