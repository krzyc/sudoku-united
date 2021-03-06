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

Container {
    id: root

    property string text
    property alias value: dropDown.selectedValue
    default property alias options: dropDown.options

    layout: StackLayout { orientation: LayoutOrientation.TopToBottom }

    topPadding: 16
    bottomPadding: 16
    leftPadding: 16
    rightPadding: 16

    preferredWidth: 768

    Label {
        textStyle.fontSize: FontSize.Default

        text: root.text
    }

    DropDown {
        id: dropDown
    }
}