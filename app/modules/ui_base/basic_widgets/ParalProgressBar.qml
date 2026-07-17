pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Shapes

Item {
    id: root

    // --- 公开 API ---
    property real value: 50.0
    property real maxValue: 100.0

    property real minVisualProgress: 0.015
    property real progress: {
        if (value <= 0.0) { return 0.0 }
        if (value >= maxValue) { return 1.0 }

        return Math.max(minVisualProgress, value / maxValue);
    }

    property bool barFromRight: false
    property int textAlignment: Qt.AlignLeft

    property real slantWidth: 20.0

    property real borderRadius: 0.0
    property real radiusTL: borderRadius
    property real radiusTR: borderRadius
    property real radiusBR: borderRadius
    property real radiusBL: borderRadius

    property color bgColor: "#333333"
    property color fillColor: "#FF3B30"
    property color borderColor: "#1A1A1A"
    property real borderWidth: 2.0


    // --- 文本属性与微调 ---
    property alias text: infoText.text
    property alias font: infoText.font
    property alias textColor: infoText.color
    property real textPadding: 10.0
    property real textOffsetX: 0.0
    property real textOffsetY: 0.0

    // --- 尺寸推导 ---
    // implicit 作为无人管时的“默认建议尺寸”，保证绝不会被文字撑破
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


    }

    // --- UI 渲染 ---
    Shape {
        id: renderItem

        anchors.fill: parent
        anchors.margins: root.borderWidth / 2.0
        antialiasing: true

        asynchronous: true

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

        FullShapePath {
            strokeWidth: 0
            fillGradient: LinearGradient {
                x1: internal.gStartX; y1: internal.gStartY
                x2: internal.gEndX; y2: internal.gEndY

                GradientStop { position: 0.0; color: root.progress > 0 ? root.fillColor : "transparent" }
                GradientStop { position: root.progress; color: root.progress > 0 ? root.fillColor : "transparent" }
                GradientStop { position: Math.min(1.0, root.progress + 0.0001); color: root.progress >= 1.0 ? root.fillColor : "transparent" }
                GradientStop { position: 1.0; color: root.progress >= 1.0 ? root.fillColor : "transparent" }
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
