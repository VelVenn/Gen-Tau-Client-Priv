pragma Singleton

import QtQuick

QtObject {
    enum Flag {
        UNKNOWN,
        REMOTE,
        LOCAL
    }

    function connModeToCnLiteral(flag) {
        switch (flag) {
        case ConnState.Flag.REMOTE:
            return "远程连接";
        case ConnState.Flag.LOCAL:
            return "本地连接";
        default:
            return "无";
        }
    }

    function connModeToLiteral(flag) {
        switch (flag) {
        case ConnState.Flag.REMOTE:
            return "Remote";
        case ConnState.Flag.LOCAL:
            return "Local";
        default:
            return "None";
        }
    }
}
