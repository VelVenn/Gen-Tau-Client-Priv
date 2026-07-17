pragma Singleton

import QtCore
import QtQuick 

QtObject {
    id: root

    property bool showCrosshair: true
    property bool showKeyHint: true
    property bool showSpdAndAmmo: true

    readonly property QtObject _storage: Settings {
        category: "uiPref"

        property alias showCrosshair: root.showCrosshair
        property alias showKeyHint: root.showKeyHint
        property alias showSpdAndAmmo: root.showSpdAndAmmo
    }
}
