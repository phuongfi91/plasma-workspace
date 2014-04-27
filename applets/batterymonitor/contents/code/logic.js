/*
 *   Copyright 2011 Sebastian Kügler <sebas@kde.org>
 *   Copyright 2012 Viranch Mehta <viranch.mehta@gmail.com>
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

var powermanagementDisabled = false;

function updateCumulative() {
    return
    var sum = 0;
    var count = 0;
    var charged = true;
    var present = false;
    for(var i = 0; i < batteriesModel.count; ++i) {
        console.log("LOGICGETTING BATTERY NUMBER " + i + " OF " + batteriesModel.count)
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
}

function plasmoidStatus() {
    return
    var status = PlasmaCore.Types.PassiveStatus;
    if (powermanagementDisabled) {
        status = PlasmaCore.Types.ActiveStatus;
    }

    if (batteriesModel.cumulativePresent) {
        if (!batteriesModel.allCharged) {
            status = PlasmaCore.Types.ActiveStatus;
        }
    } else if (batteriesModel.count > 0) { // in case your mouse gets low
        if (batteriesModel.cumulativePercent && batteriesModel.cumulativePercent <= 10) {
            status = PlasmaCore.Types.NeedsAttentionStatus;
        }
    }
    return status;
}

function lowestBattery() {
    return
    if (batteriesModel.count == 0) {
        return;
    }

    var lowestPercent = 100;
    var lowestBattery;
    for(var i=0; i<batteries.count; i++) {
        var b = batteries.get(i);
        if (b["Percent"] && b["Percent"] < lowestPercent) {
            lowestPercent = b["Percent"];
            lowestBattery = b;
        }
    }
    return b;
}

function nameForBattery(batteryData) {
    if (batteryData.powerSupply) {
        // FIXME Use proper enum
        if (batteryData.type == 2) {
            return i18n("UPS Battery")
        }
        return i18n("Battery")
    }
    if (batteryData.vendor && batteryData.product) {
        return i18nc("Placeholders are vendor and model", "%1 %2", batteryData.vendor, batteryData.product)
    }
    if (batteryData.vendor) {
        return model.device.vendor
    }
    if (batteryData.product) {
        return model.device.product
    }
    return batteryData.description
}


function stringForBatteryState(batteryData) {
    if (batteryData.present) {
        switch(batteryData.chargeState) {
            // FIXME Use proper enums
            case 0: return i18n("Not Charging")
            case 1: return i18n("Charging")
            case 2: return i18n("Discharging")
            case 3: return i18n("Fully Charged")
        }
    } else {
        return i18nc("Battery is currently not present in the bay","Not present");
    }
}

function iconForBattery(batteryData,pluggedIn) {
    switch(batteryData.type) {
        // FIXME Use proper enums
        case 9: // MonitorBattery
            return "video-display";
        case 4:
            return "input-mouse";
        case 5:
            return "input-keyboard";
        case 1:
            return "pda";
        case 8:
            return "phone";
        case 7:
            return "camera-photo"; // Could be webcam?
        case 2: // Ups
            return "battery"
        case 0: // Unknown
            return "battery"
        case 3: // Primary
            var p = batteryData.chargePercent

            if (p <= 5) {
                return "dialog-warning"
            } else if (batteryData.powerSupply) {
                return "computer-laptop"
            } else {
                return "battery"
            }
    }
}

function updateTooltip() {
    return
    var image = "";
    var text = "";
    if (batteriesModel.count === 0) {
        image = "battery-missing";
        if (!powermanagementDisabled) {
            text = i18n("No batteries available");
        }
    } else {
        var hasPowerSupply = false;

        text = "<table style='white-space: nowrap'>";
        for(var i = 0; i < batteriesModel.count; ++i) {
            var b = batteriesModel.get(i);
            text += "<tr>";
            text += "<td align='right'>" + i18nc("Placeholder is battery name", "%1:", nameForBattery(b)) + " </td>";
            text += "<td><b>";
            if (b.present) {
                text += i18nc("Placeholder is battery percentage", "%1%", b.chargePercent);
            } else {
                text += i18n("N/A")
            }
            text += "</b></td>";
            text += "</tr>";

            if (b.powerSupply) {
                hasPowerSupply = true;
            }
        }
        text += "</table>";

        if (hasPowerSupply) {
            var b = [];
            b["Type"] = "Battery";
            b["Percent"] = batteries.cumulativePercent;
            image = iconForBattery(b, pmSource.data["AC Adapter"]["Plugged in"] ? true : false);
        } else {
            var b = lowestBattery();
            image = iconForBattery(b, false);
        }
    }

    if (powermanagementDisabled) {
        text += i18n("Power management is disabled");
    }
    batteries.tooltipText = text;
    batteries.tooltipImage = image;
}

function batteryItemTooltip(batteryData, pluggedIn) {
    var text = "";

    var remainingTime = (pluggedIn ? batteryData.timeToFull : batteryData.timeToEmpty) * 1000;

    // FIXME Use proper enum
    if (batteryData.powerSupply && batteryData.chargeState != 0 && batteryData.chargeState != 3 && remainingTime > 0) {
        text += "<tr>";
        text += "<td align='right'>" + (pluggedIn ? i18n("Time To Full:") : i18n("Time To Empty:")) + " </td>";
        text += "<td><b>" + formats.formatSpelloutDuration(remainingTime) + "</b></td>";
        text += "</tr>";
    }

    if (batteryData.powerSupply && batteryData.capacity > 0) {
        text += "<tr>";
        text += "<td align='right'>" + i18n("Capacity:") + " </td>";
        text += "<td><b>" + i18nc("Placeholder is battery capacity", "%1%", batteryData.capacity) + "</b></td>"
        text += "</tr>";
    }

    // Non-powersupply batteries usually have a name consisting of the vendor and model name
    if (batteryData.powerSupply) {
        if (batteryData.vendor) {
            text += "<tr>";
            text += "<td align='right'>" + i18n("Vendor:") + " </td>";
            text += "<td><b>" + batteryData.vendor + "</b></td>";
            text += "</tr>";
        }

        if (batteryData.product) {
            text += "<tr>";
            text += "<td align='right'>" + i18n("Model:") + " </td>";
            text += "<td><b>" + batteryData.product + "</b></td>";
            text += "</tr>";
        }
    }

    //text += "<tr><td align='right'>Additional Row: </td><td><b>This is just for testing</b></td></tr>";

    if (text != "") {
        return "<table style='white-space: nowrap'>" + text + "</table>";
    }
    return "";
}

function updateBrightness(dialogItem, source) {
    if (!source.data["PowerDevil"]) {
        return;
    }

    // we don't want passive brightness change send setBrightness call
    dialogItem.disableBrightnessUpdate = true;
    if (typeof source.data["PowerDevil"]["Screen Brightness"] === 'number') {
        dialogItem.screenBrightness = source.data["PowerDevil"]["Screen Brightness"];
        dialogItem.screenBrightnessPercentage = source.data["PowerDevil"]["Screen Brightness"];
    }
    if (typeof source.data["PowerDevil"]["Keyboard Brightness"] === 'number') {
        dialogItem.keyboardBrightness = source.data["PowerDevil"]["Keyboard Brightness"];
        dialogItem.keyboardBrightnessPercentage = source.data["PowerDevil"]["Keyboard Brightness"];
    }
    dialogItem.disableBrightnessUpdate = false;
}
