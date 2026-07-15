import QtQuick
import QtQuick.Controls

import Gentau.Foundation

TabButton {
    id: root
    property Item _tabBar: null

    property alias fontFamily: buttonText.font.family
    property alias fontPxSize: buttonText.font.pixelSize
    property alias fontItalic: buttonText.font.italic

    property color textCheckedColor: "white"
    property color textHoverColor: "#CCCCCC"
    property color textNormalColor: "#AAAAAA"

    property color bgHoverColor: Qt.alpha("white", 0.08)
    
    function findTabBar() {
        if (_tabBar) return;
        var p = root.parent
        while (p) {
            if (p.objectName === "customSlideTabBar") {
                _tabBar = p
                return
            }
            p = p.parent
        }
    }

    onParentChanged: findTabBar()
    Component.onCompleted: findTabBar()
    
    anchors.verticalCenter: parent ? parent.verticalCenter : undefined
    height: _tabBar ? _tabBar.availableHeight : (parent ? parent.height : implicitHeight)
    
    width: {
        if (_tabBar && _tabBar.hasOwnProperty("tabFillWidth") && _tabBar.tabFillWidth && _tabBar.count > 0) {
            return (_tabBar.availableWidth - _tabBar.spacing * (_tabBar.count - 1)) / _tabBar.count;
        }
        return implicitWidth;
    }

    property color baseColor: Qt.lighter(Style.grayBlue, 1.5)

    contentItem: Text {
        id: buttonText

        text: root.text
        font.family: Style.notoSansSC.font.family
        font.pixelSize: 16
        font.bold: root.checked
        
        color: {
            if (root.checked) return root.textCheckedColor
            if (root.enabled && root.hovered) return root.textHoverColor
            return root.textNormalColor
        }
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

        Behavior on color { ColorAnimation { duration: 150 } }
    }

    background: Rectangle {
        radius: 8
        // 选中时完全透明，因为视觉背景由外部 TabBar 提供
        // 未选中且 Hover 并且组件可用时，给一个极微弱的底色
        color: (root.enabled && root.hovered && !root.checked) ? root.bgHoverColor : "transparent"
        Behavior on color { ColorAnimation { duration: 150 } }
    }
}
