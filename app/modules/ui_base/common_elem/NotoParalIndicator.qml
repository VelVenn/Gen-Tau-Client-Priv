import QtQuick

import Gentau.BasicWidgets

ParalIndicator {
    id: root

    property int indicatorState: NotoParalIndicator.Status.OFF

    enum Status {
        OFF = 0,
        ON = 1,
        BLINKING = 2
    }

    isOn: indicatorState === 1 || indicatorState === 2
    blinkEnabled:  indicatorState === 2

    labelFont.family: Style.notoSansSC.font.family
    labelFont.pixelSize: 10

    blinkInterval: 400

    showText: true
}
