import QtQuick
import QtQuick.Controls

import Gentau.Foundation

TabButton {
    id: root

    property color baseColor: Qt.lighter(Style.grayBlue, 1.5)

    // 按钮浮起高度（负值 = 往上浮）
    // 底部桌面（阴影）始终固定，只有按钮本体在桌面之上升降
    property real yOffset: root.pressed ? 0 : (root.checked ? -1 : -3)
    Behavior on yOffset { NumberAnimation { duration: 150; easing.type: Easing.OutCubic } }

    contentItem: Text {
        text: root.text
        font.family: Style.notoSansSC.font.family
        font.pixelSize: 16
        font.bold: root.checked
        color: {
            if (root.checked) return "white"
            if (root.enabled && root.hovered) return "#CCCCCC"
            return "#AAAAAA"
        }
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

        // 文字跟随本体升降
        transform: Translate { y: root.yOffset }

        Behavior on color { ColorAnimation { duration: 150 } }
    }

    background: Item {
        // 1. 阴影：固定在桌面上，不移动
        Rectangle {
            anchors.fill: parent
            radius: 8
            color: Qt.alpha("black", 0.5)

            // 浮得越高阴影越浓，贴地时几乎消失
            opacity: root.checked ? 0.15 : ((root.enabled && root.hovered) ? 0.8 : 0.5)
            Behavior on opacity { NumberAnimation { duration: 150 } }
        }

        // 2. 按钮本体：在桌面之上升降
        Rectangle {
            id: body
            anchors.fill: parent
            radius: 8

            // 只有本体移动，底部桌面不动
            transform: Translate { y: root.yOffset }

            color: {
                if (root.checked) return Qt.alpha(root.baseColor, 0.85)
                if (root.enabled && root.hovered) return Qt.alpha(Qt.lighter(root.baseColor, 1.3), 0.75)
                return Qt.alpha(root.baseColor, 0.6)
            }

            border.color: {
                if (root.checked) return Qt.alpha("white", 0.25)
                if (root.enabled && root.hovered) return Qt.alpha("white", 0.4)
                return Qt.alpha("white", 0.15)
            }
            border.width: 1

            Behavior on color { ColorAnimation { duration: 150 } }
            Behavior on border.color { ColorAnimation { duration: 150 } }
        }
    }
}
