pragma Singleton

import QtCore
import QtQuick 

QtObject {
    id: root

    property bool showCrosshair: true
    property bool showKeyHint: true
    property bool showSpdAndAmmo: true
    
    property bool windowedMode: true
    property bool vtUpsideDown: false

    readonly property QtObject _storage: Settings {
        category: "uiPref"

        property alias showCrosshair: root.showCrosshair
        property alias showKeyHint: root.showKeyHint
        property alias showSpdAndAmmo: root.showSpdAndAmmo

        property alias windowedMode: root.windowedMode
        property alias vtUpsideDown: root.vtUpsideDown
    }
}
