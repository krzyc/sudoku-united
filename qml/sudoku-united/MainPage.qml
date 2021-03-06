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
import "UIConstants.js" as UIConstants


Page {
    id: mainPage
    anchors.fill: parent

    orientationLock: PageOrientation.LockPortrait

    tools: ToolBarLayout {
        id: commonTools
        ToolIcon { platformIconId: "toolbar-view-menu";
             anchors.right: parent===undefined ? undefined : parent.right
             onClicked: (myMenu.status == DialogStatus.Closed) ? myMenu.open() : myMenu.close()
        }
    }

    Menu {
        id: myMenu
        visualParent: pageStack
        MenuLayout {
            MenuItem {
                text: "Highscore"
                onClicked:{
                    var component = Qt.createComponent("HighscoreView.qml")
                    pageStack.push(component, {highscoreModel: gameInstance.highscore});
                }
            }
            MenuItem {
                text: "Settings"
                onClicked:{
                    var component = Qt.createComponent("Settings.qml")
                    pageStack.push(component, {'pageStack': pageStack});
                }
            }
            MenuItem {
                text: "About"
                onClicked:{
                    var component = Qt.createComponent("About.qml")
                    pageStack.push(component, {});
                }
            }
        }
    }

    BackgroundItem{}

    Item{
        width: 300; height: 350
        anchors.centerIn: parent
        Image{
            id: logo_grid
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            width: 300; height: 300
            source: "qrc:/logo_grid_300x300.png"
        }
        Text{
            id: logo_text
            anchors.top: logo_grid.bottom
            anchors.topMargin: 5
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Sudoku United"
            font.family: UIConstants.FONT_FAMILY_BOLD
            font.pixelSize: 30
            font.bold: true
            color: "white"
        }
    }

    ButtonRow {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: mainPage.bottom
        anchors.bottomMargin: 30
        exclusive: false

        Button {
            id: createButton
            text: "Play"
            checkable: false
            onClicked: {
                var component = Qt.createComponent("CreateGameView.qml");
                pageStack.push(component);
            }
        }

        Button {
            text: "Join"
            checkable: false
            onClicked: {
                var component;
                var parameters = {};
                if (!gameInstance.settings.showedJoinHelp) {
                    component = Qt.createComponent("HelpView.qml");
                    parameters = { firstUse: true }
                } else {
                    component = Qt.createComponent("JoinView.qml");
                }

                pageStack.push(component, parameters);
            }
        }
    }
}
