pragma Singleton

import QtQuick

QtObject {
    id: root

    readonly property color redColor: Qt.rgba(0.859, 0.196, 0.196, 1)
    readonly property color blueColor: Qt.rgba(0.086, 0.392, 0.902, 1)
    readonly property color lightBlue: Qt.rgba(0.584, 0.914, 0.941, 1)
    readonly property color metallicGold: '#e68a0d'
    readonly property color grayColor: Qt.rgba(0.18, 0.157, 0.157, 1)

    readonly property color lightGreen: '#baed99'
    readonly property color lightDirt: '#ebd222'
    readonly property color lightCrimson: '#f27c0b'
    readonly property color lightFire: '#ff4b15'

    readonly property color grayBlue: '#3C3C49'
    readonly property color lighterGrayBlue: "#4f4f5b"

    readonly property color meiPink: Qt.rgba(0.929, 0.184, 0.557, 1)

    readonly property FontLoader orbitronFL: FontLoader {
        source: 'fonts/Orbitron.ttf'
    }

    readonly property FontLoader oxaniumFL: FontLoader {
        source: 'fonts/Oxanium.ttf'
    }

    readonly property FontLoader firaCodeFL: FontLoader {
        source: 'fonts/FiraCode.ttf'
    }

    readonly property FontLoader materialIconFL: FontLoader {
        source: 'fonts/MaterialSymbolsSharp.ttf'
    }

    readonly property FontLoader notoSansSC: FontLoader {
        source: 'fonts/NotoSansSC.ttf'
    }
}
