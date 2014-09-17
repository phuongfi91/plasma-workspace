/*
 * Copyright 2013 Kai Uwe Broulik <kde@privat.broulik.de>
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
import QtQuick.Controls 1.2 as QtControls
import QtQuick.Layouts 1.0

import org.kde.plasma.private.digitalclock 1.0
import org.kde.plasma.core 2.0 as PlasmaCore

Item {
    id: timeZonesPage
    width: parent.width
    height: parent.height
/*
    property alias cfg_boldText: boldCheckBox.checked
    property string cfg_timeFormat: ""
    property alias cfg_italicText: italicCheckBox.checked

    property alias cfg_showTimezone: showTimezone.checked
    property alias cfg_showSeconds: showSeconds.checked

    property alias cfg_showDate: showDate.checked
    property string cfg_dateFormat: "shortDate"*/

    TimeZoneModel {
        id: timeZones
    }

    // This is just for getting the column width
    QtControls.CheckBox {
        id: checkbox
        visible: false
    }

    ColumnLayout {
        anchors.fill: parent

        QtControls.TextField {
            id: filter
            Layout.fillWidth: true
            placeholderText: i18n("Search Time Zones")
        }

        QtControls.TableView {
            id: timeZoneView

            Layout.fillWidth: true
            Layout.fillHeight: true

            model: PlasmaCore.SortFilterModel {
                sourceModel: timeZones
                filterRole: "city"
                filterRegExp: filter.text
            }

            QtControls.TableViewColumn {
                role: "checked"
                width: checkbox.width
                delegate: Item {
                    anchors.fill: parent

                    QtControls.CheckBox {
                        anchors.centerIn: parent
                        onCheckedChanged: {
                            model.checked = checked;
                        }
                    }
                }
                resizable: false
                movable: false
            }
            QtControls.TableViewColumn {
                role: "region"
                title: i18n("Region")
            }
            QtControls.TableViewColumn {
                role: "comment"
                title: i18n("Comment")
            }
            QtControls.TableViewColumn {
                role: "city"
                title: i18n("City")
            }
        }

        RowLayout {
            Layout.fillWidth: true
            QtControls.Label {
                text: i18n("Default time zone:")
            }

            QtControls.ComboBox {

            }
        }

        RowLayout {
            Layout.fillWidth: true
            QtControls.CheckBox {
                text: i18n("Switch time zone with mouse wheel")
            }
        }

    }


/*
    QtLayouts.ColumnLayout {
        QtControls.GroupBox {
            title: i18n("Appearance")
            flat: true

            QtLayouts.ColumnLayout {
                QtControls.CheckBox {
                    id: boldCheckBox
                    text: i18n("Bold text")
                }

                QtControls.CheckBox {
                    id: italicCheckBox
                    text: i18n("Italic text")
                }
            }
        }

        QtControls.GroupBox {
            title: i18n("Information")
            flat: true

            QtLayouts.ColumnLayout {
                QtControls.CheckBox {
                    id: showSeconds
                    text: i18n("Show seconds")
                }

                QtControls.CheckBox {
                    id: showTimezone
                    text: i18n("Show time zone")
                }

                QtControls.CheckBox {
                    id: showDate
                    text: i18n("Show date")
                }

                QtLayouts.RowLayout {
                    QtControls.Label {
                        text: i18n("Date format")
                    }

                    QtControls.ComboBox {
                        id: dateFormat
                        enabled: showDate.checked
                        textRole: "label"
                        model: [
                            {
                                'label': i18n("Long date"),
                                'name': "longDate"
                            },
                            {
                                'label': i18n("Short date"),
                                'name': "shortDate"
                            },
                            {
                                'label': i18n("Narrow date"),
                                'name': "narrowDate"
                            }
                        ]
                        onCurrentIndexChanged: cfg_dateFormat = model[currentIndex]["name"]

                        Component.onCompleted: {
                            for (var i = 0; i < model.length; i++) {
                                if (model[i]["name"] == plasmoid.configuration.dateFormat) {
                                    dateFormat.currentIndex = i;
                                }
                            }
                        }
                    }
                }
            }
        }
    }*/
}
