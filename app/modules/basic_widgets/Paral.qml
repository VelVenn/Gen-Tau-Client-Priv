import QtQuick
import QtQuick.Shapes

import Gentau.Foundation

Item {
    id: root

    // --- 外观与几何属性 ---
    property color bgColor: "#F0F0F0"
    property real slantWidth: 10.0

    property int gradientPreset: GradientPreset.NoPreset
    property color baseColor: bgColor

    // 统一定义与分别定义圆角 (优先级：分别 > 统一)
    property real borderRadius: 0.0
    property real radiusTL: borderRadius
    property real radiusTR: borderRadius
    property real radiusBR: borderRadius
    property real radiusBL: borderRadius

    // 边框
    property color borderColor: "transparent"
    property real borderWidth: 0.0

    property alias gradient: path.fillGradient

    readonly property var presetGradient: {
        if (gradientPreset === GradientPreset.LighterOnLeft) {
            return internal.lighterOnLeft
        }
        if (gradientPreset === GradientPreset.LighterOnRight) {
            return internal.lighterOnRight
        }
        if (gradientPreset === GradientPreset.LighterOnTop || gradientPreset === GradientPreset.LighterOnUp) {
            return internal.lighterOnTop
        }
        if (gradientPreset === GradientPreset.LighterOnBottom || gradientPreset === GradientPreset.LighterOnDown) {
            return internal.lighterOnBottom
        }
        if (gradientPreset === GradientPreset.LighterCenter) {
            return internal.lighterCenter
        }
        return null
    }

    QtObject {
        id: internal

        property LinearGradient lighterOnLeft: LinearGradient {
            x1: 0; y1: 0
            x2: root.width; y2: 0

            GradientStop { position: 0.0; color: Qt.alpha(root.baseColor, 0.3) }
            GradientStop { position: 0.4; color: Qt.alpha(root.baseColor, 0.5) }
            GradientStop { position: 0.8; color: Qt.alpha(root.baseColor, 0.8) }
            GradientStop { position: 1.0; color: Qt.alpha(root.baseColor, 1.0) }
        }

        property LinearGradient lighterOnRight: LinearGradient {
            x1: 0; y1: 0
            x2: root.width; y2: 0

            GradientStop { position: 0.0; color: Qt.alpha(root.baseColor, 1.0) }
            GradientStop { position: 0.2; color: Qt.alpha(root.baseColor, 0.8) }
            GradientStop { position: 0.6; color: Qt.alpha(root.baseColor, 0.5) }
            GradientStop { position: 1.0; color: Qt.alpha(root.baseColor, 0.3) }
        }

        property LinearGradient lighterCenter: LinearGradient {
            x1: 0; y1: 0
            x2: root.width; y2: 0

            GradientStop { position: 0.0; color: Qt.alpha(root.baseColor, 0.8) }
            GradientStop { position: 0.2; color: Qt.alpha(root.baseColor, 0.6) }
            GradientStop { position: 0.5; color: Qt.alpha(root.baseColor, 0.4) }
            GradientStop { position: 0.8; color: Qt.alpha(root.baseColor, 0.6) }
            GradientStop { position: 1.0; color: Qt.alpha(root.baseColor, 0.8) }
        }

        property LinearGradient lighterOnTop: LinearGradient {
            x1: 0; y1: 0
            x2: 0; y2: root.height

            GradientStop { position: 0.0; color: Qt.alpha(root.baseColor, 0.3) }
            GradientStop { position: 0.4; color: Qt.alpha(root.baseColor, 0.5) }
            GradientStop { position: 0.8; color: Qt.alpha(root.baseColor, 0.8) }
            GradientStop { position: 1.0; color: Qt.alpha(root.baseColor, 1.0) }
        }

        property LinearGradient lighterOnBottom: LinearGradient {
            x1: 0; y1: 0
            x2: 0; y2: root.height

            GradientStop { position: 0.0; color: Qt.alpha(root.baseColor, 1.0) }
            GradientStop { position: 0.2; color: Qt.alpha(root.baseColor, 0.8) }
            GradientStop { position: 0.6; color: Qt.alpha(root.baseColor, 0.5) }
            GradientStop { position: 1.0; color: Qt.alpha(root.baseColor, 0.3) }
        }

        // 图元尺寸缓存
        property real w: root.width
        property real h: root.height
        property real dx: Math.abs(root.slantWidth)
        property real safeSlant: Math.min(dx, w)
        property real tOff: root.slantWidth >= 0 ? safeSlant : 0.0
        property real bOff: root.slantWidth < 0 ? safeSlant : 0.0
        property real usableW: Math.max(0, w - safeSlant)

        // 极限圆角约束计算
        property real edgeLen: Math.max(0.001, Math.sqrt(dx * dx + h * h))
        property real maxR: Math.min(usableW / 2.0, edgeLen / 2.0)

        property real rTL: Math.max(0.001, Math.min(root.radiusTL, maxR))
        property real rTR: Math.max(0.001, Math.min(root.radiusTR, maxR))
        property real rBR: Math.max(0.001, Math.min(root.radiusBR, maxR))
        property real rBL: Math.max(0.001, Math.min(root.radiusBL, maxR))

        // 锐角与钝角切线系数
        property real fAcute: (edgeLen + dx) / h
        property real fObtuse: (edgeLen - dx) / h
        property int sDir: root.slantWidth >= 0 ? 1 : -1

        // 顶角切点偏移量计算
        property real tTL: rTL * (root.slantWidth >= 0 ? fObtuse : fAcute)
        property real tTR: rTR * (root.slantWidth >= 0 ? fAcute : fObtuse)
        property real tBR: rBR * (root.slantWidth >= 0 ? fObtuse : fAcute)
        property real tBL: rBL * (root.slantWidth >= 0 ? fAcute : fObtuse)

        // 斜边单位向量分量
        property real dxR: dx / edgeLen
        property real dyR: h / edgeLen

        // 8 个切点的精确坐标计算
        property real p1_hx: tOff + tTL;                    property real p1_hy: 0
        property real p2_hx: w - bOff - tTR;                property real p2_hy: 0
        property real p2_sx: w - bOff - sDir * tTR * dxR;   property real p2_sy: tTR * dyR
        property real p3_sx: w - tOff + sDir * tBR * dxR;   property real p3_sy: h - tBR * dyR
        property real p3_hx: w - tOff - tBR;                property real p3_hy: h
        property real p4_hx: bOff + tBL;                    property real p4_hy: h
        property real p4_sx: bOff + sDir * tBL * dxR;       property real p4_sy: h - tBL * dyR
        property real p1_sx: tOff - sDir * tTL * dxR;       property real p1_sy: tTL * dyR
    }

    Shape {
        id: bgShape
        anchors.fill: parent
        anchors.margins: root.borderWidth / 2.0
        
        // 开启抗锯齿与异步数学计算（保护 CPU 不掉帧）
        antialiasing: true
        asynchronous: true

        ShapePath {
            id: path

            fillColor: root.bgColor
            fillGradient: root.presetGradient
            strokeColor: root.borderColor
            strokeWidth: root.borderWidth

            // 画笔起始点 (Top-Left 水平点)
            startX: internal.p1_hx; startY: internal.p1_hy

            // 顶部线 -> Top-Right 圆角
            PathLine { x: internal.p2_hx; y: internal.p2_hy }
            PathArc {
                x: internal.p2_sx; y: internal.p2_sy
                radiusX: internal.rTR; radiusY: internal.rTR
                direction: PathArc.Clockwise
            }

            // 右侧斜边 -> Bottom-Right 圆角
            PathLine { x: internal.p3_sx; y: internal.p3_sy }
            PathArc {
                x: internal.p3_hx; y: internal.p3_hy
                radiusX: internal.rBR; radiusY: internal.rBR
                direction: PathArc.Clockwise
            }

            // 底部线 -> Bottom-Left 圆角
            PathLine { x: internal.p4_hx; y: internal.p4_hy }
            PathArc {
                x: internal.p4_sx; y: internal.p4_sy
                radiusX: internal.rBL; radiusY: internal.rBL
                direction: PathArc.Clockwise
            }

            // 左侧斜边 -> Top-Left 圆角 (闭合)
            PathLine { x: internal.p1_sx; y: internal.p1_sy }
            PathArc {
                x: internal.p1_hx; y: internal.p1_hy
                radiusX: internal.rTL; radiusY: internal.rTL
                direction: PathArc.Clockwise
            }
        }
    }
}
