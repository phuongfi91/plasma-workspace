/*
 *   Copyright 2013-2015 by Sebastian Kügler <sebas@kde.org>
 *   Copyright 2015 by Martin Klapetek <mklapetek@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  2.010-1301, USA.
 */

import QtQuick 2.0
import QtQuick.Layouts 1.1
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.plasma.private.digitalclock 1.0

Item {
    id: tooltipContentItem

    property Item toolTip
    property int preferredTextWidth: units.gridUnit * 20

    Layout.minimumWidth: childrenRect.width + units.gridUnit
    Layout.minimumHeight: childrenRect.height + units.gridUnit
    Layout.maximumWidth: childrenRect.width + units.gridUnit
    Layout.maximumHeight: childrenRect.height + units.gridUnit

    function timeForZone(zone) {
        // get the time for the given timezone from the dataengine
        var now = dataSource.data[zone]["DateTime"];
        // get current UTC time
        var msUTC = now.getTime() + (now.getTimezoneOffset() * 60000);
        // add the dataengine TZ offset to it
        var dateTime = new Date(msUTC + (dataSource.data[zone]["Offset"] * 1000));

        return Qt.formatTime(dateTime, main.timeFormat)
    }

    function nameForZone(zone) {
        // add the timezone string to the clock
        var timezoneString = plasmoid.configuration.displayTimezoneAsCode ? dataSource.data[zone]["Timezone Abbreviation"]
                                                                          : TimezonesI18n.i18nCity(dataSource.data[zone]["Timezone City"]);

        print(timezoneString);
        return timezoneString;
    }

    RowLayout {

        anchors {
            left: parent.left
            top: parent.top
            margins: units.gridUnit / 2
        }

        spacing: units.largeSpacing

        PlasmaCore.IconItem {
            id: tooltipIcon
            source: "preferences-system-time"
            Layout.alignment: Qt.AlignTop
            visible: true
            implicitWidth: units.iconSizes.medium
            Layout.preferredWidth: implicitWidth
            Layout.preferredHeight: implicitWidth
        }

        ColumnLayout {

            PlasmaExtras.Heading {
                id: tooltipMaintext
                level: 3
                Layout.minimumWidth: Math.min(implicitWidth, preferredTextWidth)
                Layout.maximumWidth: preferredTextWidth
                elide: Text.ElideRight
                text: Qt.formatDate(tzDate,"dddd")
            }

            PlasmaComponents.Label {
                id: tooltipSubtext
                Layout.minimumWidth: Math.min(implicitWidth, preferredTextWidth)
                Layout.maximumWidth: preferredTextWidth
                text: Qt.formatDate(tzDate, dateFormatString)
                opacity: 0.6
            }

            GridLayout {
                Layout.minimumWidth: Math.min(implicitWidth, preferredTextWidth)
                Layout.maximumWidth: preferredTextWidth
                columns: 2

                Repeater {
                    model: {
                        // The timezones need to be duplicated in the array
                        // because we need their data twice - once for the name
                        // and once for the time and the Repeater delegate cannot
                        // be one Item with two Labels because that wouldn't work
                        // in a grid then
                        var timezones = [];
                        for (var i = 0; i < plasmoid.configuration.selectedTimeZones.length; i++) {
                            timezones.push(plasmoid.configuration.selectedTimeZones[i]);
                            timezones.push(plasmoid.configuration.selectedTimeZones[i]);
                        }

                        return timezones;
                    }

                    PlasmaComponents.Label {
                        id: timezone
                        Layout.fillWidth: true

                        wrapMode: Text.NoWrap
                        text: index % 2 == 0 ? nameForZone(modelData) : timeForZone(modelData)
                        font.weight: modelData === plasmoid.configuration.lastSelectedTimezone ? Font.Bold : Font.Normal
                        horizontalAlignment: index % 2 == 0 ? Text.AlignRight : Text.AlignLeft
                        opacity: 0.6
                    }
                }
            }
        }
    }
}
