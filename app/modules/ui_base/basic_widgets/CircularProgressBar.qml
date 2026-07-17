import QtQuick
import QtQuick.Shapes

Item {
    id: root

    property real value: 50.0
    property real maxValue: 100.0
    property real radius: 50.0
    property real minVisualProgress: 0.0

    property real progress: {
        if (value <= 0.0) return 0.0;
        if (value >= maxValue) return 1.0;
        return Math.max(minVisualProgress, value / maxValue);
    }

    property int startDeg: 0
    property int endDeg: 360
    property real ringWidth: 4.0
    property real bgRingWidth: ringWidth

    property color bgColor: "gray"
    property color fillColor: "red"

    implicitWidth: (radius * 2.0) + Math.max(ringWidth, bgRingWidth)
    implicitHeight: implicitWidth

    QtObject {
        id: internal

        property real cx: root.width / 2.0
        property real cy: root.height / 2.0

        property real rawSweep: root.endDeg - root.startDeg
        property real sweep: {
            if (rawSweep > 359.99) return 359.99;
            if (rawSweep < -359.99) return -359.99;
            return rawSweep;
        }

        property bool isCCW: sweep < 0
        property real absSweep: Math.abs(sweep)

        property real fillFraction: (root.progress * absSweep) / 360.0

        property real gradAngle: isCCW ? (90.0 - root.startDeg)
                                       : (90.0 - root.startDeg - (root.progress * absSweep))

        function generateDonut(r, w) {
            if (r <= 0 || w <= 0) return "";

            let outR = r + w / 2.0;
            let inR = r - w / 2.0;
            let angleOffset = -90.0;
            let startRad = (root.startDeg + angleOffset) * Math.PI / 180.0;
            let endRad = (root.startDeg + sweep + angleOffset) * Math.PI / 180.0;

            let largeArc = absSweep > 180 ? 1 : 0;
            let sweepFlag = isCCW ? 0 : 1;

            let osX = cx + outR * Math.cos(startRad);
            let osY = cy + outR * Math.sin(startRad);
            let oeX = cx + outR * Math.cos(endRad);
            let oeY = cy + outR * Math.sin(endRad);

            let isX = cx + inR * Math.cos(startRad);
            let isY = cy + inR * Math.sin(startRad);
            let ieX = cx + inR * Math.cos(endRad);
            let ieY = cy + inR * Math.sin(endRad);

            return "M " + isX + " " + isY +
                   " L " + osX + " " + osY +
                   " A " + outR + " " + outR + " 0 " + largeArc + " " + sweepFlag + " " + oeX + " " + oeY +
                   " L " + ieX + " " + ieY +
                   " A " + inR + " " + inR + " 0 " + largeArc + " " + (isCCW ? 1 : 0) + " " + isX + " " + isY +
                   " Z";
        }

        property string bgPath: generateDonut(root.radius, root.bgRingWidth)
        property string fgPath: generateDonut(root.radius, root.ringWidth)
    }

    Shape {
        anchors.fill: parent
        // layer.enabled: true
        // layer.samples: 4
        antialiasing: true
        asynchronous: true

        ShapePath {
            strokeWidth: 0
            fillColor: root.bgColor
            PathSvg { path: internal.bgPath }
        }

        ShapePath {
            strokeWidth: 0
            fillGradient: ConicalGradient {
                centerX: internal.cx
                centerY: internal.cy
                angle: internal.gradAngle

                GradientStop { position: 0.0; color: root.progress > 0 ? root.fillColor : "transparent" }
                GradientStop { position: internal.fillFraction; color: root.progress > 0 ? root.fillColor : "transparent" }
                GradientStop { position: Math.min(1.0, internal.fillFraction + 0.00001); color: "transparent" }
                GradientStop { position: 1.0; color: "transparent" }
            }
            PathSvg { path: internal.fgPath }
        }
    }
}
