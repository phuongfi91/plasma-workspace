/*
 * Copyright 2013 Heena Mahour <heena393@gmail.com>
 * Copyright 2013 Sebastian Kügler <sebas@kde.org>
 * Copyright 2013 Martin Klapetek <mklapetek@kde.org>
 * Copyright 2014 David Edmundson <davidedmundson@kde.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.6
import org.kde.plasma.components 2.0 as Components

ClockBase {
    id: main

    contentItem: _contentItem

    Item {
        id: _contentItem
        anchors.verticalCenter: main.verticalCenter

        Grid {
            id: labelsGrid

            rows: 1
            horizontalItemAlignment: Grid.AlignHCenter
            verticalItemAlignment: Grid.AlignVCenter

            flow: Grid.TopToBottom
            columnSpacing: units.smallSpacing

            Rectangle {
                height: 0.8 * sizehelper.height
                width: 1
                visible: main.showDate && main.oneLineMode

                color: theme.textColor
                opacity: 0.4
            }

            Components.Label  {
                id: timeLabel

                font {
                    family: plasmoid.configuration.fontFamily || theme.defaultFont.family
                    weight: plasmoid.configuration.boldText ? Font.Bold : theme.defaultFont.weight
                    italic: plasmoid.configuration.italicText
                    pixelSize: 1024
                }
                minimumPixelSize: 1

                text: {
                    // get the time for the given timezone from the dataengine
                    print("smconfig"+plasmoid.configuration.lastSelectedTimezone)
                    var now = dataSource.data[plasmoid.configuration.lastSelectedTimezone]["DateTime"];
                    // get current UTC time
                    var msUTC = now.getTime() + (now.getTimezoneOffset() * 60000);
                    // add the dataengine TZ offset to it
                    var currentTime = new Date(msUTC + (dataSource.data[plasmoid.configuration.lastSelectedTimezone]["Offset"] * 1000));

                    main.currentTime = currentTime;
                    return Qt.formatTime(currentTime, main.timeFormat);
                }

                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }

            Components.Label {
                id: timezoneLabel

                font.weight: timeLabel.font.weight
                font.italic: timeLabel.font.italic
                font.pixelSize: 1024
                minimumPixelSize: 1

                visible: text.length > 0
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        Components.Label {
            id: dateLabel

            visible: main.showDate

            font.family: timeLabel.font.family
            font.weight: timeLabel.font.weight
            font.italic: timeLabel.font.italic
            font.pixelSize: 1024
            minimumPixelSize: 1

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }
}
