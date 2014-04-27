/*
 *   Copyright 2011 Sebastian Kügler <sebas@kde.org>
 *   Copyright 2011 Viranch Mehta <viranch.mehta@gmail.com>
 *   Copyright 2013 Kai Uwe Broulik <kde@privat.broulik.de>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 2.0
import QtQuick.Layouts 1.1
import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.solid 1.0 as Solid
import "plasmapackage:/code/logic.js" as Logic

Item {
    id: batterymonitor
    Plasmoid.switchWidth: units.gridUnit * 10
    Plasmoid.switchHeight: units.gridUnit * 10

    LayoutMirroring.enabled: Qt.application.layoutDirection == Qt.RightToLeft
    LayoutMirroring.childrenInherit: true

    Plasmoid.toolTipMainText: ""
    Plasmoid.toolTipSubText: batteriesModel.tooltipText
    Plasmoid.icon: batteriesModel.tooltipImage

    Component.onCompleted: {
        updateLogic();
    }

    function updateLogic() {
        print("UPDATE LOGIC HAS BEEN CALLED")
        var sum = 0;
        var count = 0;
        var charged = true;
        var present = false;
        print("I will now count batteries")

        print(batteriesModel)
//return
        console.log(batteriesModel.get(0))




                    return
        for(var i = 0; i < 2; ++i) {
            var b = batteriesModel.get(i)
            console.log(b)
        }
        return

        for(var i = 0; i < batteriesModel.count; ++i) {
            console.log("BLAGETTING BATTERY NUMBER " + i + " OF " + batteriesModel.count)
            var b = batteriesModel.get(i)
            if (!b.powerSupply) {
                continue;
            }
            if (b.present) {
                sum += b.chargePercent;
                present = true;
            }
            // FIXME Use proper enum
            // NOTE Maybe we should consider noCharge as full too?
            if (b.chargeState != 3) {
                charged = false;
            }
            ++count;
        }

        if (count > 0) {
            batteriesModel.cumulativePercent = Math.round(sum/count);
        } else {
            // We don't have any power supply batteries
            // Use the lowest value from any battery
            if (batteriesModel.count > 0) {
                var b = lowestBattery();
                batteriesModel.cumulativePercent = b.chargePercent;
            } else {
                batteriesModel.cumulativePercent = 0;
            }
        }

        batteriesModel.cumulativePresent = present;
        batteriesModel.allCharged = charged;
return
        //Logic.updateCumulative();
        plasmoid.status = Logic.plasmoidStatus();

        // this helps updating the status throughout the applet
        // without this, the applet doesn't switch from passive
        // to active. -- FIXME.
//        print(plasmoid.status);
        Logic.updateTooltip();
    }

    Plasmoid.compactRepresentation: CompactRepresentation {
        batteries: batteriesModel
    }

    Solid.Devices {
        id: batteriesModel
        query: "IS Battery"

        Component.onDestruction: print("DESTROYED")

        Component.onCompleted: {
            console.log("SOLID DEVICES READY")
            updateLogic() // FIXME React to changes too
        }
        onDeviceAdded: {
            console.log("DIEWAISS ÄDDED")
            updateLogic()
        }

        property int cumulativePercent
        property bool cumulativePresent
        // true  --> all batteries charged
        // false --> one of the batteries charging/discharging
        property bool allCharged

        property string tooltipText
        property string tooltipImage
    }

    /*property QtObject pmSource: PlasmaCore.DataSource {
        id: pmSource
        engine: "powermanagement"
        connectedSources: sources
        onDataChanged: {
            updateLogic();
        }
        onSourceAdded: {
            if (source == "Battery0") {
                disconnectSource(source);
                connectSource(source);
            }
        }
        onSourceRemoved: {
            if (source == "Battery0") {
                disconnectSource(source);
            }
        }
    }

    property QtObject batteries: PlasmaCore.SortFilterModel {
        id: batteries
        filterRole: "Is Power Supply"
        sortOrder: Qt.DescendingOrder
        sourceModel: PlasmaCore.SortFilterModel {
            sortRole: "Pretty Name"
            sortOrder: Qt.AscendingOrder
            sortCaseSensitivity: Qt.CaseInsensitive
            sourceModel: PlasmaCore.DataModel {
                dataSource: pmSource
                sourceFilter: "Battery[0-9]+"

                onDataChanged: updateLogic()
            }
        }

        property int cumulativePercent
        property bool cumulativePluggedin

        property bool allCharged
        property string tooltipText
        property string tooltipImage
    }*/

    Plasmoid.fullRepresentation: PopupDialog {
        id: dialogItem
        Layout.minimumWidth: units.iconSizes.medium * 9
        Layout.minimumHeight: dialogItem.implicitHeight
        Layout.maximumHeight: dialogItem.implicitHeight

        model: batteriesModel//batteries

        anchors.fill: parent
        focus: true

        property bool disableBrightnessUpdate: false

        isBrightnessAvailable: pmSource.data["PowerDevil"]["Screen Brightness Available"] ? true : false
        isKeyboardBrightnessAvailable: pmSource.data["PowerDevil"]["Keyboard Brightness Available"] ? true : false

        remainingTime: Number(pmSource.data["Battery"]["Remaining msec"])

        pluggedIn: pmSource.data["AC Adapter"]["Plugged in"]

        Component.onCompleted: {
            dialogItem.forceActiveFocus();
        }

        Connections {
            target: pmSource
            onDataChanged : {
                Logic.updateBrightness(dialogItem, pmSource);
            }
        }

        onBrightnessChanged: {
            if (disableBrightnessUpdate) {
                return;
            }
            var service = pmSource.serviceForSource("PowerDevil");
            var operation = service.operationDescription("setBrightness");
            operation.brightness = screenBrightness;
            service.startOperationCall(operation);
        }
        onKeyboardBrightnessChanged: {
            if (disableBrightnessUpdate) {
                return;
            }
            var service = pmSource.serviceForSource("PowerDevil");
            var operation = service.operationDescription("setKeyboardBrightness");
            operation.brightness = keyboardBrightness;
            service.startOperationCall(operation);
        }
        property int cookie1: -1
        property int cookie2: -1
        onPowermanagementChanged: {
            var service = pmSource.serviceForSource("PowerDevil");
            if (checked) {
                var op1 = service.operationDescription("stopSuppressingSleep");
                op1.cookie = cookie1;
                var op2 = service.operationDescription("stopSuppressingScreenPowerManagement");
                op2.cookie = cookie2;

                var job1 = service.startOperationCall(op1);
                job1.finished.connect(function(job) {
                    cookie1 = -1;
                });

                var job2 = service.startOperationCall(op2);
                job2.finished.connect(function(job) {
                    cookie2 = -1;
                });
            } else {
                var reason = i18n("The battery applet has enabled system-wide inhibition");
                var op1 = service.operationDescription("beginSuppressingSleep");
                op1.reason = reason;
                var op2 = service.operationDescription("beginSuppressingScreenPowerManagement");
                op2.reason = reason;

                var job1 = service.startOperationCall(op1);
                job1.finished.connect(function(job) {
                    cookie1 = job.result;
                });

                var job2 = service.startOperationCall(op2);
                job2.finished.connect(function(job) {
                    cookie2 = job.result;
                });
            }
            Logic.powermanagementDisabled = !checked;
            updateLogic();
        }
    }
}
