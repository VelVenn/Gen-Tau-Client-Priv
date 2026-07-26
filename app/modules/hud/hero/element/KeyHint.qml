pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic

import Gentau.Foundation
import Gentau.CommonElem

RowLayout {
    id: root

    required property list<string> keyTexts
    required property string hintText

    property string actionText: "按"

    property bool isPressed: false
    property int interactMode: KeyHint.Mode.Click
    property real pressProgress: 0.0
    property int progressKeyIndex: root.keyTexts.length - 1

    property font labelFont: Qt.font({
        family: Style.notoSansSC.font.family,
        pixelSize: 13,
        bold: false
    })
    property color labelColor: "white"

    property font keyFont: Qt.font({
        family: Style.notoSansSC.font.family,
        pixelSize: 12,
        bold: true
    })
    property color keyBaseColor: Qt.alpha(Qt.lighter(Style.grayBlue, 1.2), 0.8)
    property color keyTextColor: "white"
    property color keyProgressColor: Qt.rgba(1, 1, 1, 0.6)

    property real keySpacing: 4

    enum Mode {
        Click,
        Delay
    }

    spacing: 8

    Label {
        text: root.actionText
        color: root.labelColor
        font: root.labelFont
    }

    RowLayout {
        spacing: root.keySpacing

        Repeater {
            model: root.keyTexts

            RowLayout {
                id: keyDelegate

                required property int index
                required property string modelData

                readonly property bool isProgressKey:
                    keyDelegate.index === root.progressKeyIndex

                spacing: root.keySpacing

                Label {
                    text: "+"
                    color: root.labelColor
                    font: root.keyFont
                    visible: keyDelegate.index > 0
                }

                KeyBadge {
                    text: keyDelegate.modelData
                    font: root.keyFont

                    baseColor: root.keyBaseColor
                    textColor: root.keyTextColor
                    progressColor: root.keyProgressColor

                    isPressed: root.isPressed
                    interactMode:
                        root.interactMode === KeyHint.Mode.Delay && keyDelegate.isProgressKey
                            ? KeyBadge.Mode.Delay
                            : KeyBadge.Mode.Click
                    pressProgress: keyDelegate.isProgressKey ? root.pressProgress : 0.0
                }
            }
        }
    }

    Label {
        text: root.hintText
        color: root.labelColor
        font: root.labelFont
    }
}
