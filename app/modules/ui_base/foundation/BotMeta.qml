pragma Singleton

import QtQuick

QtObject {
    enum BotType {
        UNKNOWN,
        HERO,
        ENG,
        INF,
        AIR,
        SENTRY,
        DART,
        RADAR
    }

    enum BotCamp {
        UNKNOWN,
        RED,
        BLUE
    }

    readonly property ListModel comboModel: ListModel {
        ListElement {
            text: "红1 - 英雄"
            idx: 1
        }
        ListElement {
            text: "蓝1 - 英雄"
            idx: 101
        }
        ListElement {
            text: "红2 - 工程"
            idx: 2
        }
        ListElement {
            text: "蓝2 - 工程"
            idx: 102
        }
        ListElement {
            text: "红3 - 步兵"
            idx: 3
        }
        ListElement {
            text: "蓝3 - 步兵"
            idx: 103
        }
        ListElement {
            text: "红4 - 步兵"
            idx: 4
        }
        ListElement {
            text: "蓝4 - 步兵"
            idx: 104
        }
        ListElement {
            text: "红6 - 无人机"
            idx: 6
        }
        ListElement {
            text: "蓝6 - 无人机"
            idx: 106
        }
        ListElement {
            text: "红7 - 哨兵"
            idx: 7
        }
        ListElement {
            text: "蓝7 - 哨兵"
            idx: 107
        }
        ListElement {
            text: "红9 - 雷达"
            idx: 9
        }
        ListElement {
            text: "蓝9 - 雷达"
            idx: 109
        }
    }

    readonly property int rHero: 1
    readonly property int rEng: 2
    readonly property int rInf3: 3
    readonly property int rInf4: 4
    readonly property int rInf5: 5
    readonly property int rAir: 6
    readonly property int rSentry: 7
    readonly property int rDart: 8
    readonly property int rRadar: 9

    readonly property int bHero: 101
    readonly property int bEng: 102
    readonly property int bInf3: 103
    readonly property int bInf4: 104
    readonly property int bInf5: 105
    readonly property int bAir: 106
    readonly property int bSentry: 107
    readonly property int bDart: 108
    readonly property int bRadar: 109

    readonly property var shooterPerformMode: {
        "Cooldown": 1,
        "Burst": 2,
        "HeroMelee": 3,
        "HeroRemote": 4
    }

    readonly property var chassisPerformMode: {
        "Health": 1,
        "Power": 2,
        "HeroMelee": 3,
        "HeroRemote": 4
    }

    function toBotShooterPerfModeStr(flag) {
        switch (flag) {
        case 1:
            return "冷却优先";
        case 2:
            return "爆发优先";
        case 3:
            return "近战优先";
        case 4:
            return "远程优先";
        default:
            return "无";
        }
    }

    function toBotChassisPerfModeStr(flag) {
        switch (flag) {
        case 1:
            return "血量优先";
        case 2:
            return "功率优先";
        case 3:
            return "近战优先";
        case 4:
            return "远程优先";
        default:
            return "无";
        }
    }

    function toBotName(idx) {
        switch (idx) {
        case 1:
        case 101:
            return '英雄';
        case 2:
        case 102:
            return '工程';
        case 3:
        case 103:
        case 4:
        case 104:
        case 5:
        case 105:
            return '步兵';
        case 6:
        case 106:
            return '无人机';
        case 7:
        case 107:
            return '哨兵';
        case 8:
        case 108:
            return '飞镖';
        case 9:
        case 109:
            return '雷达';
        default:
            return '未知';
        }
    }

    function toBotNameEng(idx) {
        switch (idx) {
        case 1:
        case 101:
            return 'Hero';
        case 2:
        case 102:
            return 'Engineer';
        case 3:
        case 103:
        case 4:
        case 104:
        case 5:
        case 105:
            return 'Infantry';
        case 6:
        case 106:
            return 'Aerial';
        case 7:
        case 107:
            return 'Sentry';
        case 8:
        case 108:
            return 'Dart';
        case 9:
        case 109:
            return 'Radar';
        default:
            return 'Unknown';
        }
    }

    function toBotAvatarIdxString(idx) {
        switch (idx) {
        case 1:
        case 101:
            return 'Hero';
        case 2:
        case 102:
            return 'Eng';
        case 3:
        case 103:
        case 4:
        case 104:
        case 5:
        case 105:
            return 'Inf';
        case 6:
        case 106:
            return 'Air';
        case 7:
        case 107:
            return 'Sentry';
        case 8:
        case 108:
            return 'Dart';
        case 9:
        case 109:
            return 'Radar';
        default:
            return 'Default';
        }
    }

    function toBotCampAndIdxStr(idx) {
        if (idx >= 1 && idx <= 9) {
            return "R" + idx;
        }

        if (idx >= 101 && idx <= 109) {
            return "B" + (idx - 100);
        }

        return "None";
    }

    function idStrToBotIdx(idStr) {
        const idx = Number(idStr);

        if (!Number.isInteger(idx)) {
            return 0;
        }

        if ((idx >= 1 && idx <= 9) || (idx >= 101 && idx <= 109)) {
            return idx;
        }

        return 0;
    }

    function toBotType(idx) {
        switch (idx) {
        case 1:
        case 101:
            return BotMeta.BotType.HERO;
        case 2:
        case 102:
            return BotMeta.BotType.ENG;
        case 3:
        case 103:
        case 4:
        case 104:
        case 5:
        case 105:
            return BotMeta.BotType.INF;
        case 6:
        case 106:
            return BotMeta.BotType.AIR;
        case 7:
        case 107:
            return BotMeta.BotType.SENTRY;
        case 8:
        case 108:
            return BotMeta.BotType.DART;
        case 9:
        case 109:
            return BotMeta.BotType.RADAR;
        default:
            return BotMeta.BotType.UNKNOWN;
        }
    }

    function toRealBotIdx(idx) {
        if (idx >= 1 && idx <= 9) {
            return idx;
        }

        if (idx >= 101 && idx <= 109) {
            return idx - 100;
        }

        return 0;
    }

    function toBotCamp(idx) {
        if (idx >= 1 && idx <= 11) {
            return BotMeta.BotCamp.RED;
        } else if (idx >= 101 && idx <= 111) {
            return BotMeta.BotCamp.BLUE;
        }

        return BotMeta.BotCamp.UNKNOWN;
    }

    function toBotLvlRomaString(lv) {
        switch (lv) {
        case 1:
            return 'I';
        case 2:
            return 'II';
        case 3:
            return 'III';
        case 4:
            return 'IV';
        case 5:
            return 'V';
        case 6:
            return 'VI';
        case 7:
            return 'VII';
        case 8:
            return 'VIII';
        case 9:
            return 'IX';
        case 10:
            return 'X';
        }
    }
}
