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

import bb.cascades 1.0
import sudoku 1.0

ListItemComponent {
    id: root

    property Settings settings

    type: "gametimer"

    SwitchComponent {
        text: "Game timer"
        subText: "Shows the elapsed time in-game"

        checked: ListItem.component.settings.showGameTimer

        onCheckedChanged: ListItem.component.settings.showGameTimer = checked
    }
}