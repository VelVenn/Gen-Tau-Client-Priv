pragma Singleton

import QtQuick

QtObject {
    id: root

    enum Type {
        NoPreset,
        LighterOnLeft,
        LighterOnRight,
        LighterCenter,
        LighterOnTop,
        LighterOnBottom,
        LighterOnUp,
        LighterOnDown
    }
}
