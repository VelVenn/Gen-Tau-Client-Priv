import QtQuick
import QtQuick.Shapes

import Gentau.Foundation

Item {
    id: root

    property alias text: badgeText.text
    property alias font: badgeText.font
    property alias textColor: badgeText.color

    property alias badgeGradient: path.fillGradient

    // --- 背景与边框样式 ---
    property color bgColor: "#F0F0F0"
    property color borderColor: Qt.darker(bgColor, 1.2)
    property real borderWidth: 1.0
    property real borderRadius: 4.0

    // --- 梯形专属属性 ---
    property real bottomIndent: 12.0 // 底部向内收缩的斜度

    // --- 布局控制 ---
    property real horizontalPadding: 24.0
    property real verticalPadding: 4.0

    property real textOffsetY: 0.0

    implicitWidth: badgeText.implicitWidth + (horizontalPadding * 2)
    implicitHeight: badgeText.implicitHeight + (verticalPadding * 2)

    Shape {
        id: bgShape
        anchors.fill: parent
        // 核心细节：整体向内收缩半个边框宽度
        anchors.margins: root.borderWidth / 2.0
        antialiasing: true

        asynchronous: true

        property real w: width
        property real h: height
        property real ind: root.bottomIndent
        property real edgeLen: Math.max(0.001, Math.sqrt(ind * ind + h * h))

        // --- 核心改动 2：升级为 PathArc 标准圆弧运算，确保内边框圆润 ---
        // 安全保护：计算极限圆角半径，防止图形缩进过深时路径交叉破裂
        property real maxR: Math.min(
            h / 2.0,
            (w / 2.0) * (h / (edgeLen + ind)),
            ((w - 2 * ind) / 2.0) * (h / (edgeLen - ind))
        )
        property real r: Math.min(root.borderRadius, Math.max(0, maxR))

        // 精确计算各个顶点到真正“圆弧切点”的距离 (T)
        property real tTop: r * (edgeLen + ind) / h
        property real tBot: r * (edgeLen - ind) / h

        // 计算斜边上的坐标偏移量
        property real slantTopX: tTop * (ind / edgeLen)
        property real slantTopY: tTop * (h / edgeLen)
        property real slantBotX: tBot * (ind / edgeLen)
        property real slantBotY: tBot * (h / edgeLen)

        ShapePath {
            id: path

            fillColor: root.bgColor
            strokeColor: root.borderColor
            strokeWidth: root.borderWidth

            // 绘制起点：左上角圆弧结束点
            startX: bgShape.tTop
            startY: 0

            // 1. 顶部水平线
            PathLine { x: bgShape.w - bgShape.tTop; y: 0 }

            // 2. 右上角：标准正圆弧 (替代旧版的贝塞尔 PathQuad)
            PathArc {
                x: bgShape.w - bgShape.slantTopX; y: bgShape.slantTopY
                radiusX: bgShape.r; radiusY: bgShape.r
                direction: PathArc.Clockwise
            }

            // 3. 右侧斜线
            PathLine {
                x: bgShape.w - bgShape.ind + bgShape.slantBotX
                y: bgShape.h - bgShape.slantBotY
            }

            // 4. 右下角：标准正圆弧
            PathArc {
                x: bgShape.w - bgShape.ind - bgShape.tBot; y: bgShape.h
                radiusX: bgShape.r; radiusY: bgShape.r
                direction: PathArc.Clockwise
            }

            // 5. 底部水平线
            PathLine { x: bgShape.ind + bgShape.tBot; y: bgShape.h }

            // 6. 左下角：标准正圆弧
            PathArc {
                x: bgShape.ind - bgShape.slantBotX; y: bgShape.h - bgShape.slantBotY
                radiusX: bgShape.r; radiusY: bgShape.r
                direction: PathArc.Clockwise
            }

            // 7. 左侧斜线
            PathLine { x: bgShape.slantTopX; y: bgShape.slantTopY }

            // 8. 左上角：标准正圆弧，闭合
            PathArc {
                x: bgShape.tTop; y: 0
                radiusX: bgShape.r; radiusY: bgShape.r
                direction: PathArc.Clockwise
            }
        }
    }

    Text {
        id: badgeText
        anchors.centerIn: parent

        // 默认值下沉：外部不赋值时的基础状态
        text: "text"
        color: "#333333"
        font.family: Style.notoSansSC.font.family
        font.pixelSize: 16.0
        font.weight: Font.Medium

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

        anchors.verticalCenter: root.verticalCenter
        anchors.verticalCenterOffset: root.textOffsetY
    }
}
