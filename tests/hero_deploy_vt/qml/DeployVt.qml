import QtQuick
import QtQuick.Controls
import QtQuick.Window

import org.freedesktop.gstreamer.Qt6GLVideoItem 1.0

Window {
    id: mainWin

    visible: true
    width: 1280
    height: 720

    color: 'black'
    title: qsTr("Hero Deploy VT")

    property int butPadding: 20

    GstGLQt6VideoItem {
        id: video
        objectName: "videoItem"
        anchors.centerIn: parent
        width: parent.width
        height: parent.height
    }

    // Rectangle {
    //     id: bottomFill 

    //     height: 10
    //     width: 50

    //     color: 'transparent'

    //     anchors.horizontalCenter: parent.horizontalCenter
    //     anchors.bottom: parent.bottom
    //     anchors.bottomMargin: 10
    // }

    Button {
        id: redHeroButton

        text: "Red"

        anchors.right: blueHeroButton.left 
        anchors.verticalCenter: blueHeroButton.verticalCenter

        anchors.rightMargin: mainWin.butPadding

        onClicked: vtRecv.requestClientSwitch("1")
    }

    Button {
        id: blueHeroButton 

        text: "Blue"

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom

        onClicked: vtRecv.requestClientSwitch("101")

        anchors.bottomMargin: 10
    }

    Button {
        id: restartButton 

        text: "Restart"

        anchors.left: blueHeroButton.right
        anchors.verticalCenter: blueHeroButton.verticalCenter

        anchors.leftMargin: mainWin.butPadding

        onClicked: vtRecv.requestRestartVidRend()
    }


    Shortcut {
        sequence: "Escape"
        onActivated: Qt.quit()
    }
}
