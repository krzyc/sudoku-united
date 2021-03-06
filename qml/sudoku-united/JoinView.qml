/**
    This file is part of Sudoku United.

    Sudoku United is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Sudoku United is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Sudoku United.  If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import sudoku 1.0
import "UIConstants.js" as UIConstants
import "UIFunctions.js" as UIFunctions

Page {
    id: joinView

    property bool searchedOnce: false

    orientationLock: PageOrientation.LockPortrait

    tools: ToolBarLayout {
        id: joinToolbar
        ToolIcon {
            iconId: "toolbar-back"
            onClicked: pageStack.pop()
        }

        ToolButton {
            text: "Join game"
            anchors.centerIn: parent
            enabled: list.currentItem != null
            onClicked: {
                loadingOverlay.open()
                gameInfoModel.autoRefresh = false
                gameInstance.join(list.currentItem.myData.gameInfo)
            }
        }

        ToolButton {
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            text: "?"
            width: 52
            height: 52
            anchors.rightMargin: UIConstants.DEFAULT_MARGIN
            onClicked: {
                var component = Qt.createComponent("HelpView.qml");
                pageStack.push(component);
            }
        }
    }

    InfoBanner {
        id: infoBanner
        z: 10
    }

    Connections {
        target: gameInstance

        onJoinFailed: {
            console.log("Failed")
            loadingOverlay.close()
            gameInfoModel.autoRefresh = true
            if (reason.length > 0)
                infoBanner.text = "Connection to player failed: " + reason
            else
                infoBanner.text = "Connection to player failed."
            infoBanner.show()
        }

        onGameChanged: {
            if (!gameInstance.game) {
                gameInfoModel.autoRefresh = true
                return;
            }

            if (gameInstance.game.board) {
                loadingOverlay.close()
                pageStack.push(gameView);
            }
        }
    }

    Connections{
        target: gameInfoModel

        onStateChanged: {
            if (!joinView.searchedOnce){
                joinView.searchedOnce = true
            }
        }
    }

    LoadingOverlay {
        id: loadingOverlay
        text: "Joining game"
        showCancelButton: true

        onRejected: {
            gameInstance.cancelJoin()
        }
    }

    property string gameNameProperty: ""
    property variant gameInfoModel

    Component.onCompleted: gameInfoModel = gameInstance.discoverGames()
    Component.onDestruction: gameInfoModel.destroy()

    Component {
        id: highlight

        Rectangle {
            id: selectionRectangle
            width: joinView.width; height: 40
            color: "lightsteelblue"; radius: 5
            y: list.currentItem ? list.currentItem.y : 0
            Behavior on y {
                SpringAnimation {
                    spring: 3
                    damping: 0.2
                }
            }
        }
    }

    Header {
        id: topStatusBar

        state: {
            var stateCode = gameInfoModel.state;

            switch (stateCode) {
            case GameInfoModel.Discovering:
                return "discovering";
            case GameInfoModel.Complete:
                return "complete";
            }

            return "";
        }

        states: [
            State {
                name: "discovering"
                PropertyChanges { target: topStatusBar; text: "Discovering games" }
                PropertyChanges { target: topStatusBar; busy: true }
            },
            State {
                name: "complete"
                PropertyChanges { target: topStatusBar; text: "Discovery complete" }
                PropertyChanges { target: topStatusBar; busy: false }
            }
        ]

    }

    ListView {
        id: list
        anchors.top: topStatusBar.bottom
        anchors.left: topStatusBar.left
        anchors.right: topStatusBar.right
        anchors.bottom: parent.bottom

        model: gameInfoModel
        delegate: Item {
            property variant myData: model
            id: deviceItem
            clip: true
            width: parent.width; height: UIConstants.LIST_ITEM_HEIGHT_DEFAULT
            anchors.horizontalCenter: parent.horizontalCenter
            state: "up"

            Text {
                id: gameName
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 16
                text: name
                font.pixelSize: UIConstants.FONT_SLARGE;
            }

            Rectangle{
                anchors.bottom: parent.bottom
                width: parent.width - 10
                anchors.horizontalCenter: parent.horizontalCenter
                height: 1
                radius: 1
                color: "lightgrey"
            }

            MouseArea {
                id: mouseArea;
                anchors.fill: deviceItem
                onClicked: {
                    list.currentIndex = index
                    gameNameProperty = name
                }
            }
        }

        highlight: highlight
        highlightFollowsCurrentItem: true
        focus: true
        clip: true
    }

    ScrollDecorator {
        flickableItem: list
    }

    Rectangle {
        id: discoveryFailedOverlay
        width: 400
        height: UIFunctions.totalComponentHeight(warningImage) + UIFunctions.totalComponentHeight(noGamesDiscoveredLabel) + 16
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        color: "white"
        radius: 7
        visible: joinView.searchedOnce && list.count === 0 ? true : false

        Image{
            id: warningImage
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 10
            source: "image://theme/icon-s-transfer-error"
        }

        Label{
            id: noGamesDiscoveredLabel
            anchors.top: warningImage.bottom
            anchors.topMargin: 10
            anchors.left: parent.left
            anchors.leftMargin: 16
            anchors.right: parent.right
            anchors.rightMargin: 16
            text: "No games were found.\nPlease make sure that the other player has made the game public and that your Google Talk/Jabber account is online or that Bluetooth is enabled."
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            platformStyle: LabelStyle{
                fontFamily: UIConstants.FONT_FAMILY_LIGHT
            }
        }
    }

}

