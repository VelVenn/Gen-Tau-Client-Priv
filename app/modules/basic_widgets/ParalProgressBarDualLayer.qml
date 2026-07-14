pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Shapes

// [basic comp] 单层多重渐变渲染组件
// SRP: 只负责基于单层 Shape 的多重颜色渲染，没有任何业务逻辑
Item {
    id: root

    // --- 公开 API ---
    // 严格遵守 Rule 9: 只开放接口，不在此组件内部用 = 反写这些属性
    property real value
    property real secondaryValue
    property real maxValue
    property real minVisualProgress

    // 初始化默认参数，使用Component.onCompleted避免 : 与 = 混用
    Component.onCompleted: {
        if (value === undefined) value = 50.0
        if (secondaryValue === undefined) secondaryValue = 70.0
        if (maxValue === undefined) maxValue = 100.0
        if (minVisualProgress === undefined) minVisualProgress = 0.015
    }

    property real progress: {
        if (value <= 0.0) { return 0.0 }
        if (value >= maxValue) { return 1.0 }
        return Math.max(minVisualProgress, value / maxValue);
    }

    property real secondaryProgress: {
        if (secondaryValue <= 0.0) { return 0.0 }
        if (secondaryValue >= maxValue) { return 1.0 }
        return Math.max(minVisualProgress, secondaryValue / maxValue);
    }

    property bool barFromRight: false
    property int textAlignment: Qt.AlignLeft

    property real slantWidth: 20.0

    property real radiusTL: 0.0
    property real radiusTR: 0.0
    property real radiusBR: 0.0
    property real radiusBL: 0.0

    function setAllRad(r: real) {
        radiusTL = r
        radiusTR = r
        radiusBR = r
        radiusBL = r
    }

    property color bgColor: "#333333"
    property color fillColor: "#FF3B30"
    property color secondaryFillColor: "#FFFFFF"
    property color borderColor: "#1A1A1A"
    property real borderWidth: 2.0

    property alias text: infoText.text
    property alias font: infoText.font
    property alias textColor: infoText.color
    property real textPadding: 10.0
    property real textOffsetX: 0.0
    property real textOffsetY: 0.0

    implicitWidth: Math.max(200, infoText.text !== "" ? infoText.contentWidth + (internal.dx * 2) + textPadding * 2 : 0)
    implicitHeight: Math.max(24, infoText.text !== "" ? infoText.contentHeight + 4 : 0)

    // --- 私有计算域 ---
    QtObject {
        id: internal
        property real w: root.width
        property real h: root.height
        property real dx: Math.abs(root.slantWidth)
        property real safeSlant: Math.min(dx, w)
        property real tOff: root.slantWidth >= 0 ? safeSlant : 0.0
        property real bOff: root.slantWidth < 0 ? safeSlant : 0.0
        property real usableW: Math.max(0, w - safeSlant)

        property real edgeLen: Math.max(0.001, Math.sqrt(dx * dx + h * h))
        property real maxR: Math.min(usableW / 2.0, edgeLen / 2.0)

        property real rTL: Math.max(0.001, Math.min(root.radiusTL, maxR))
        property real rTR: Math.max(0.001, Math.min(root.radiusTR, maxR))
        property real rBR: Math.max(0.001, Math.min(root.radiusBR, maxR))
        property real rBL: Math.max(0.001, Math.min(root.radiusBL, maxR))

        property real fAcute: (edgeLen + dx) / h
        property real fObtuse: (edgeLen - dx) / h
        property int sDir: root.slantWidth >= 0 ? 1 : -1

        property real tTL: rTL * (root.slantWidth >= 0 ? fObtuse : fAcute)
        property real tTR: rTR * (root.slantWidth >= 0 ? fAcute : fObtuse)
        property real tBR: rBR * (root.slantWidth >= 0 ? fObtuse : fAcute)
        property real tBL: rBL * (root.slantWidth >= 0 ? fAcute : fObtuse)

        property real dxR: dx / edgeLen
        property real dyR: h / edgeLen

        property real p1_hx: tOff + tTL;                    property real p1_hy: 0
        property real p2_hx: w - bOff - tTR;                property real p2_hy: 0
        property real p2_sx: w - bOff - sDir * tTR * dxR;   property real p2_sy: tTR * dyR
        property real p3_sx: w - tOff + sDir * tBR * dxR;   property real p3_sy: h - tBR * dyR
        property real p3_hx: w - tOff - tBR;                property real p3_hy: h
        property real p4_hx: bOff + tBL;                    property real p4_hy: h
        property real p4_sx: bOff + sDir * tBL * dxR;       property real p4_sy: h - tBL * dyR
        property real p1_sx: tOff - sDir * tTL * dxR;       property real p1_sy: tTL * dyR

        property real denom: h * h + (tOff - bOff) * (tOff - bOff)
        property real gDX: usableW * h * h / denom
        property real gDY: usableW * h * (tOff - bOff) / denom
        property real gStartX: root.barFromRight ? tOff + gDX : tOff
        property real gStartY: root.barFromRight ? gDY : 0
        property real gEndX: root.barFromRight ? tOff : tOff + gDX
        property real gEndY: root.barFromRight ? 0 : gDY


        // 核心单层着色器计算 (Rule 8: 剔除重复图层，靠单一 Gradient 控制)
        property bool showSecondary: root.secondaryProgress > root.progress
        property real stop1: root.progress
        property real stop2: Math.min(1.0, root.progress + 0.0001) // 极小偏移造就硬边缘切分
        property real stop3: showSecondary ? root.secondaryProgress : stop2
        property real stop4: Math.min(1.0, stop3 + 0.0001)
    }

    // --- UI 渲染 ---
    Shape {
        anchors.fill: parent
        anchors.margins: root.borderWidth / 2.0
        antialiasing: true

        component FullShapePath : ShapePath {
            startX: internal.p1_hx; startY: internal.p1_hy
            PathLine { x: internal.p2_hx; y: internal.p2_hy }
            PathArc { x: internal.p2_sx; y: internal.p2_sy; radiusX: internal.rTR; radiusY: internal.rTR; direction: PathArc.Clockwise }
            PathLine { x: internal.p3_sx; y: internal.p3_sy }
            PathArc { x: internal.p3_hx; y: internal.p3_hy; radiusX: internal.rBR; radiusY: internal.rBR; direction: PathArc.Clockwise }
            PathLine { x: internal.p4_hx; y: internal.p4_hy }
            PathArc { x: internal.p4_sx; y: internal.p4_sy; radiusX: internal.rBL; radiusY: internal.rBL; direction: PathArc.Clockwise }
            PathLine { x: internal.p1_sx; y: internal.p1_sy }
            PathArc { x: internal.p1_hx; y: internal.p1_hy; radiusX: internal.rTL; radiusY: internal.rTL; direction: PathArc.Clockwise }
        }

        FullShapePath {
            fillColor: root.bgColor
            strokeWidth: 0
        }

        // 核心修改：唯一填充层，通过 6 个停靠点完成【主颜色】和【追血颜色】的无缝拼接
        FullShapePath {
            strokeWidth: 0
            fillGradient: LinearGradient {
                x1: internal.gStartX; y1: internal.gStartY
                x2: internal.gEndX; y2: internal.gEndY

                GradientStop { position: 0.0; color: root.progress > 0 ? root.fillColor : "transparent" }
                GradientStop { position: internal.stop1; color: root.progress > 0 ? root.fillColor : "transparent" }
                GradientStop { position: internal.stop2; color: internal.showSecondary ? root.secondaryFillColor : "transparent" }
                GradientStop { position: internal.stop3; color: internal.showSecondary ? root.secondaryFillColor : "transparent" }
                GradientStop { position: internal.stop4; color: "transparent" }
                GradientStop { position: 1.0; color: "transparent" }
            }
        }

        FullShapePath {
            fillColor: "transparent"
            strokeColor: root.borderColor
            strokeWidth: root.borderWidth
        }
    }

    Text {
        id: infoText
        text: ""
        color: "white"
        font.pixelSize: 12

        anchors.left: root.textAlignment === Qt.AlignLeft ? parent.left : undefined
        anchors.right: root.textAlignment === Qt.AlignRight ? parent.right : undefined
        anchors.horizontalCenter: root.textAlignment === Qt.AlignHCenter ? parent.horizontalCenter : undefined

        anchors.leftMargin: root.textAlignment === Qt.AlignLeft ? (Math.max(internal.tOff, internal.bOff) + root.textPadding + root.textOffsetX) : 0
        anchors.rightMargin: root.textAlignment === Qt.AlignRight ? (Math.max(internal.tOff, internal.bOff) + root.textPadding - root.textOffsetX) : 0
        anchors.horizontalCenterOffset: root.textOffsetX

        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: root.textOffsetY

        horizontalAlignment: root.textAlignment === Qt.AlignLeft
            ? Text.AlignLeft
            : (root.textAlignment === Qt.AlignRight ? Text.AlignRight : Text.AlignHCenter)
        verticalAlignment: Text.AlignVCenter
    }
}
