//
// Copyright (C) 2013-2018 University of Amsterdam
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public
// License along with this program.  If not, see
// <http://www.gnu.org/licenses/>.
//

<<<<<<< HEAD
import QtQuick 2.10
=======
import QtQuick 2.11
>>>>>>> qmlFormsB
import JASP.Theme 1.0

Button {
    id: button
<<<<<<< HEAD
=======
    readonly property string iconToLeft: "qrc:/images/arrow-left.png"
    readonly property string iconToRight: "qrc:/images/arrow-right.png"
    
    hasTabFocus: false    
    
>>>>>>> qmlFormsB
    property var leftSource;
    property var rightSource;
    property bool leftToRight: true

    property var source: leftToRight ? leftSource : rightSource;
    property var target: leftToRight ? rightSource : leftSource;

<<<<<<< HEAD
    readonly property string iconToLeft: "qrc:/images/arrow-left.png"
    readonly property string iconToRight: "qrc:/images/arrow-right.png"
    text: ""
    x: (rightSource.x + leftSource.width - width) / 2
    y: rightSource.y + rightSource.rectangleY

    width: 40
    height: 20

    Image {
        id: image
        fillMode: Image.PreserveAspectFit
        anchors.centerIn: parent
        sourceSize.height: button.background.height - 6
        height: sourceSize.height
        source: leftToRight ? iconToRight : iconToLeft
    }

=======
    image.source: leftToRight ? iconToRight : iconToLeft
    
    control.width: 40
    control.height: 20
    
    x: (rightSource.x + leftSource.width - control.width) / 2
    y: rightSource.y + rightSource.rectangleY

    
>>>>>>> qmlFormsB
    onClicked: source.moveSelectedItems(target)

    function setIconToRight() {
        if (leftSource.activeFocus)
            leftToRight = true;
    }

    function setIconToLeft() {
        if (rightSource.activeFocus)
            leftToRight = false;
    }

    function setDisabledState() {
        if (source.hasSelectedItems)
            state = "";
        else
            state = "disabled";
    }

    onSourceChanged: setDisabledState()

    Component.onCompleted: {
        rightSource.activeFocusChanged.connect(setIconToLeft);
        leftSource.activeFocusChanged.connect(setIconToRight);
        rightSource.hasSelectedItemsChanged.connect(setDisabledState);
        leftSource.hasSelectedItemsChanged.connect(setDisabledState);
        state = "disabled";
    }

}
