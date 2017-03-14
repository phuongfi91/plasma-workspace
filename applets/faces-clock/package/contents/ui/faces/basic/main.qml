/*
 * Copyright 2017 Sebastian Kügler <sebas@kde.org>
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
import ".."

ClockBase {
    Components.Label {
        id: main
        text: {
            // get the time for the given timezone from the dataengine
            var now = dataSource.data[plasmoid.configuration.lastSelectedTimezone]["DateTime"];
            // get current UTC time
            var msUTC = now.getTime() + (now.getTimezoneOffset() * 60000);
            // add the dataengine TZ offset to it
            var currentTime = new Date(msUTC + (dataSource.data[plasmoid.configuration.lastSelectedTimezone]["Offset"] * 1000));

            //main.currentTime = currentTime;
            return Qt.formatTime(currentTime, main.timeFormat);
        }
        font.pixelSize: Math.min((parent.height * 0.8), (parent.width / 4))
        anchors.centerIn: parent
    }
}
