/*
 * Copyright 2013  Bhushan Shah <bhush94@gmail.com>
 * Copyright 2013 Sebastian Kügler <sebas@kde.org>
 * Copyright 2015 Kai Uwe Broulik <kde@privat.broulik.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

import QtQuick 2.0
import QtQuick.Controls 1.0 as QtControls
import QtQuick.Layouts 1.0 as QtLayouts
import org.kde.plasma.calendar 2.0 as PlasmaCalendar
import org.kde.plasma.core 2.0 as PlasmaCore

Item {
    id: facePage
    width: childrenRect.width
    height: childrenRect.height

    signal configurationChanged

    property string cfg_clockFace: ""

    ListModel {
        id: facesModel

        ListElement {
            name: "Basic clock"
            source: "faces/BasicClock.qml"
        }
        ListElement {
            name: "Analog clock"
            source: "faces/analogclock.qml"
        }
        ListElement {
            name: "Digital clock"
            source: "faces/DigitalClock.qml"
        }
    }

    PlasmaCore.DataSource {
        id: dataSource
        engine: "time"
        connectedSources: {
            var tz  = Array()
            if (plasmoid.configuration.selectedTimeZones.indexOf("Local") === -1) {
                tz.push("Local");
            }
            return tz.concat(plasmoid.configuration.selectedTimeZones);
        }
        interval: plasmoid.configuration.showSeconds ? 1000 : 60000
        intervalAlignment: plasmoid.configuration.showSeconds ? PlasmaCore.Types.NoAlignment : PlasmaCore.Types.AlignToMinute
    }

    QtLayouts.ColumnLayout {
        anchors.left: parent.left
        QtControls.GroupBox {
            QtLayouts.Layout.fillWidth: true
            title: i18n("Font")
            flat: true

            QtLayouts.GridLayout { // there's no FormLayout :(
                columns: 2
                QtLayouts.Layout.fillWidth: true

                QtControls.Label {
                    QtLayouts.Layout.fillWidth: true
                    horizontalAlignment: Text.AlignRight
                    text: i18n("Clock face:")
                }

                QtControls.ComboBox {
                    id: clockFaceComboBox
                    QtLayouts.Layout.fillWidth: true
                    // ComboBox's sizing is just utterly broken
                    QtLayouts.Layout.minimumWidth: units.gridUnit * 10
                    model: facesModel
                    //model: ["faces/BasicClock.qml", "faces/DigitalClock.qml"]
                    textRole: "name"
                    onCurrentIndexChanged: {
                        var current = model.get(currentIndex)
                        print("Current face: " + current.name);
                        if (current) {
                            cfg_clockFace = current.source
                            facePage.configurationChanged()
                        }
                    }
                }

                // spacer, cannot do Qt.AlignTop on the font style label + rowSpan 3, otherwise looks odd
                Item {
                    QtLayouts.Layout.fillWidth: true
                    QtLayouts.Layout.rowSpan: 2
                }

                Column {
                    QtLayouts.Layout.fillWidth: true
                    QtLayouts.Layout.rowSpan: 2
                    Repeater {


                        model: facesModel

                        Rectangle {
                            width: parent.width
                            height: width * 0.6

                            color: "white"
                            QtControls.Label {
                                id: lbl
                                anchors.centerIn: parent
                                text: source
                            }

                            Loader {
                                anchors.fill: parent
                                source: lbl.text
                            }
                        }
                    }

                }
            }
        }
    }
}
