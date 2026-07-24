import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts

import Gentau.Foundation
import Gentau.ConfPanel.Element

import Gentau.Service.Conn

ScrollView {
    id: root

    clip: true

    required property ConnService connService

    readonly property int curBotIdx: BotMeta.idStrToBotIdx(root.connService.clientId)

    readonly property int curConnMode: root.connService.connMode

    property int curShooterPerfMode: 0
    property int curChassisPerfMode: 0

    component RootGroupBoxBg: Rectangle {
        color: Qt.darker(Style.grayBlue, 1.1)
        border.color: Qt.darker(Style.grayBlue, 1.5)
        border.width: 2
        radius: 5
    }

    QtObject {
        id: param

        readonly property string curIdxAbbrStr: BotMeta.toBotCampAndIdxStr(root.curBotIdx)

        readonly property color curCampColor: {
            switch (BotMeta.toBotCamp(root.curBotIdx)) {
            case BotMeta.BotCamp.RED:
                return Qt.lighter(Style.redColor, 1.2);
            case BotMeta.BotCamp.BLUE:
                return Qt.lighter(Style.blueColor, 1.4);
            default:
                return Style.lightDirt;
            }
        }

        readonly property string curConnModeStr:
            root.connService.connModeToString(root.curConnMode)

        readonly property color curConnModeColor: {
            switch (root.curConnMode) {
            case ConnService.ConnMode.Remote:
            case ConnService.ConnMode.Local:
                return Style.lightGreen;
            default:
                return Style.lightDirt;
            }
        }

        readonly property int selectedConnMode: {
            return connModeTab.currentIndex === 0
                ? ConnService.ConnMode.Remote
                : ConnService.ConnMode.Local;
        }

        readonly property bool showPerformCards: {
            let botIdx = idxCombo.currentValue;

            return [1, 101, 3, 103, 4, 104].includes(botIdx);
        }

        readonly property int curPerformCardsPage: {
            let botIdx = idxCombo.currentValue;

            if (botIdx === 1 || botIdx === 101)
                return 0;

            return 1;
        }

        readonly property int initIdxComboVal: {
            return root.curBotIdx !== 0 ? root.curBotIdx : 1;
        }

        readonly property int initConnTabIdx: {
            return root.curConnMode === ConnService.ConnMode.Local ? 1 : 0;
        }

        readonly property int heroPerformTabChoseMode: {
            if (heroPerformTab.currentIndex === 0)
                return BotMeta.shooterPerformMode.HeroMelee; // 3

            return BotMeta.shooterPerformMode.HeroRemote; // 4
        }

        readonly property int initHeroPerfTabIdx: {
            if (root.curShooterPerfMode === BotMeta.shooterPerformMode.HeroRemote)
                return 1;

            return 0;
        }

        readonly property int initInfanShooterTabIdx: {
            if (root.curShooterPerfMode === BotMeta.shooterPerformMode.Burst)
                return 1;

            return 0;
        }

        readonly property int initInfanChassisTabIdx: {
            if (root.curChassisPerfMode === BotMeta.chassisPerformMode.Power)
                return 1;

            return 0;
        }

        readonly property string curHeroPerfModeStr: {
            if (root.curShooterPerfMode < 3) {
                return "无";
            }

            return BotMeta.toBotShooterPerfModeStr(root.curShooterPerfMode);
        }

        readonly property string curInfanShooterPerfModeStr: {
            if (root.curShooterPerfMode > 2) {
                return "无";
            }

            return BotMeta.toBotShooterPerfModeStr(root.curShooterPerfMode);
        }

        readonly property string curInfanChassisPerfModeStr: {
            if (root.curChassisPerfMode > 2) {
                return "无";
            }

            return BotMeta.toBotChassisPerfModeStr(root.curChassisPerfMode);
        }
    }

    ColumnLayout {
        id: bottomLayout

        width: root.availableWidth

        GroupBox {
            id: loginCard

            Layout.fillWidth: true

            background: RootGroupBoxBg {}

            ColumnLayout {
                width: parent.width

                Label {
                    text: "登录"

                    font.family: Style.notoSansSC.font.family
                    font.pixelSize: 15

                    color: "white"

                    Layout.alignment: Qt.AlignTop
                }

                RectCombo {
                    id: idxCombo

                    Layout.fillWidth: true

                    textRole: "text"
                    valueRole: "idx"

                    model: BotMeta.comboModel

                    Component.onCompleted: {
                        var targetIdx = idxCombo.indexOfValue(param.initIdxComboVal);

                        if (targetIdx !== -1) {
                            idxCombo.currentIndex = targetIdx;
                        }
                    }
                }

                Label {
                    text: '连接模式'

                    font.family: Style.notoSansSC.font.family
                    font.pixelSize: 15

                    color: "white"
                }

                SpringSlideTabBar {
                    id: connModeTab

                    Layout.fillWidth: true

                    bgColor: 'transparent'
                    bgBorder.color: Qt.darker(Style.grayBlue, 1.5)
                    bgBorder.width: 2

                    tabFillWidth: true

                    Repeater {
                        model: ['远程连接', '本地连接']

                        SlideNavTab {
                            text: modelData
                        }
                    }

                    Component.onCompleted: {
                        connModeTab.currentIndex = param.initConnTabIdx;
                    }
                }

                Item {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 1
                }

                RowLayout {
                    Layout.fillWidth: true

                    spacing: 5

                    RectLitStatusBadge {
                        id: curloginBotIdxBadge

                        Layout.fillWidth: true
                        Layout.preferredWidth: 15

                        hintText: "当前已登录："

                        statusText: param.curIdxAbbrStr

                        statusTextColor: param.curCampColor
                    }

                    RectLitStatusBadge {
                        id: curConnModeBadge

                        Layout.fillWidth: true
                        Layout.preferredWidth: 15

                        hintText: "当前连接模式："

                        statusText: param.curConnModeStr

                        statusTextColor: param.curConnModeColor
                    }

                    RectButton {
                        id: connButton

                        Layout.fillWidth: true
                        Layout.preferredWidth: 10

                        Layout.fillHeight: true
                        text: "连        接"

                        textColor: Style.lightGreen
                        font.bold: true

                        baseBorderColor: Style.lightGreen

                        onClicked: {
                            if (idxCombo.currentIndex < 0) {
                                return;
                            }

                            root.connService.bind(
                                idxCombo.currentValue.toString(),
                                param.selectedConnMode
                            );
                        }
                    }
                }
            }
        }

        StackLayout {
            id: performCards

            Layout.fillWidth: true

            Layout.preferredHeight: children[currentIndex] ? children[currentIndex].implicitHeight : 0

            visible: param.showPerformCards

            currentIndex: param.curPerformCardsPage

            enabled: root.curBotIdx === idxCombo.currentValue

            opacity: enabled ? 1.0 : 0.6

            GroupBox {
                id: heroPerformCard

                Layout.fillWidth: true

                background: RootGroupBoxBg {}

                contentItem: ColumnLayout {
                    Label {
                        text: "性能模式"

                        font.family: Style.notoSansSC.font.family
                        font.pixelSize: 15

                        color: "white"
                    }

                    SpringSlideTabBar {
                        id: heroPerformTab

                        Layout.fillWidth: true

                        bgColor: 'transparent'
                        bgBorder.color: Qt.darker(Style.grayBlue, 1.5)
                        bgBorder.width: 2

                        tabFillWidth: true

                        Repeater {
                            model: ['近战优先', '远程优先']

                            SlideNavTab {
                                text: modelData
                            }
                        }

                        Component.onCompleted: {
                            heroPerformTab.currentIndex = param.initHeroPerfTabIdx;
                        }
                    }

                    Item {
                        Layout.preferredHeight: 0
                    }

                    RowLayout {
                        Layout.fillWidth: true

                        RectLitStatusBadge {
                            id: curHeroPerformMode

                            Layout.preferredWidth: heroPerformCard.availableWidth / 2

                            hintText: "当前性能体系："

                            statusText: param.curHeroPerfModeStr

                            statusTextFont.family: Style.notoSansSC.font.family

                            rightFillWidth: 15
                        }

                        RectButton {
                            id: heroPerformButton

                            Layout.fillWidth: true
                            Layout.fillHeight: true

                            text: "发   送   指   令"
                        }
                    }
                }
            }

            GroupBox {
                id: infanPerformCard

                Layout.fillWidth: true

                background: RootGroupBoxBg {}

                contentItem: ColumnLayout {
                    Label {
                        text: "性能模式"

                        font.family: Style.notoSansSC.font.family
                        font.pixelSize: 15

                        color: "white"
                    }

                    Item {
                        Layout.preferredHeight: 3
                    }

                    Label {
                        text: "发射机构"

                        font.family: Style.notoSansSC.font.family
                        font.pixelSize: 13

                        color: "white"
                    }

                    SpringSlideTabBar {
                        id: infanShooterPerfTab

                        Layout.fillWidth: true

                        bgColor: 'transparent'
                        bgBorder.color: Qt.darker(Style.grayBlue, 1.5)
                        bgBorder.width: 2

                        tabFillWidth: true

                        Repeater {
                            model: ['冷却优先', '爆发优先']

                            SlideNavTab {
                                text: modelData

                                fontPxSize: 14
                            }
                        }

                        Component.onCompleted: {
                            infanShooterPerfTab.currentIndex = param.initInfanShooterTabIdx;
                        }
                    }

                    Label {
                        text: "底盘系统"

                        font.family: Style.notoSansSC.font.family
                        font.pixelSize: 13

                        color: "white"
                    }

                    SpringSlideTabBar {
                        id: infanChassisPerfTab

                        Layout.fillWidth: true

                        bgColor: 'transparent'
                        bgBorder.color: Qt.darker(Style.grayBlue, 1.5)
                        bgBorder.width: 2

                        tabFillWidth: true

                        Repeater {
                            model: ['血量优先', '功率优先']

                            SlideNavTab {
                                text: modelData

                                fontPxSize: 14
                            }
                        }

                        Component.onCompleted: {
                            infanChassisPerfTab.currentIndex = param.initInfanChassisTabIdx;
                        }
                    }

                    Item {
                        Layout.preferredHeight: 0
                    }

                    RowLayout {
                        Layout.fillWidth: true

                        RectLitStatusBadge {
                            id: curInfanShooterPerformMode

                            Layout.fillWidth: true
                            Layout.preferredWidth: 40

                            hintText: "当前发射机构性能："

                            statusText: param.curInfanShooterPerfModeStr

                            statusTextFont.family: Style.notoSansSC.font.family

                            rightFillWidth: 10
                        }

                        RectLitStatusBadge {
                            id: curInfanChassisPerformMode

                            Layout.fillWidth: true
                            Layout.preferredWidth: 40

                            hintText: "当前底盘系统性能："

                            statusText: param.curInfanChassisPerfModeStr

                            statusTextFont.family: Style.notoSansSC.font.family

                            rightFillWidth: 10
                        }

                        RectButton {
                            id: infanPerformButton

                            Layout.fillWidth: true
                            Layout.preferredWidth: 20

                            Layout.fillHeight: true

                            text: "发 送 指 令"
                        }
                    }
                }
            }
        }
    }
}
