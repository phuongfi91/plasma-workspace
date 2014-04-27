/*
 *   Copyright 2012-2013 Daniel Nicoletti <dantti12@gmail.com>
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
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as Components
import org.kde.kquickcontrolsaddons 2.0 as KQuickControlsAddons
import org.kde.kcoreaddons 1.0 as KCoreAddons
import "plasmapackage:/code/logic.js" as Logic

Item {
    id: batteryItem

    width: batteryColumn.width
    height: batteryInfos.height + padding.margins.top + padding.margins.bottom

    // NOTE: According to the UPower spec this property is only valid for primary batteries, however
    // UPower seems to set the Present property false when a device is added but not probed yet
    property bool isPresent: model.device.present

    property int remainingTime

    KCoreAddons.Formats {
        id: formats
    }

    PlasmaCore.ToolTipArea {
        id: batteryTooltip
        anchors.fill: parent
        icon: batteryIcon.icon
        mainText: batteryNameLabel.text
        subText: Logic.batteryItemTooltip(model.device, pluggedIn)
    }

    Item {
        id: batteryInfos
        height: Math.max(batteryIcon.height, batteryNameLabel.height + batteryPercentBar.height)

        anchors {
            top: parent.top
            topMargin: padding.margins.top
            left: parent.left
            leftMargin: padding.margins.left
            right: parent.right
            rightMargin: padding.margins.right
        }

        KQuickControlsAddons.QIconItem {
            id: batteryIcon
            width: units.iconSizes.medium
            height: width
            anchors {
                verticalCenter: parent.verticalCenter
                left: parent.left
            }
            icon: Logic.iconForBattery(model.device, pluggedIn)
        }

        SequentialAnimation {
          id: chargeAnimation
          // FIXME use proper enum
          running: units.longDuration > 0 && model.device.chargeState === 1 && model.device.powerSupply
          alwaysRunToEnd: true
          loops: Animation.Infinite

          NumberAnimation {
              target: batteryIcon
              properties: "opacity"
              from: 1.0
              to: 0.5
              duration: 750
              easing.type: Easing.InCubic
          }
          NumberAnimation {
              target: batteryIcon
              properties: "opacity"
              from: 0.5
              to: 1.0
              duration: 750
              easing.type: Easing.OutCubic
          }
        }

        Components.Label {
            id: batteryNameLabel
            anchors {
                verticalCenter: isPresent ? undefined : batteryIcon.verticalCenter
                top: isPresent ? parent.top : undefined
                left: batteryIcon.right
                leftMargin: 6
            }
            height: implicitHeight
            elide: Text.ElideRight
            text: Logic.nameForBattery(model.device)
        }

        Components.Label {
            id: batteryStatusLabel
            anchors {
                top: batteryNameLabel.top
                left: batteryNameLabel.right
                leftMargin: 3
            }
            text: Logic.stringForBatteryState(model.device)
            height: implicitHeight
            visible: model.device.powerSupply
            enabled: false
        }

        Components.ProgressBar {
            id: batteryPercentBar
            anchors {
                bottom: parent.bottom
                left: batteryIcon.right
                leftMargin: 6
                right: batteryPercent.left
                rightMargin: 6
            }
            minimumValue: 0
            maximumValue: 100
            visible: isPresent
            value: parseInt(model.device.chargePercent)
        }

        Components.Label {
            id: batteryPercent
            anchors {
                verticalCenter: batteryPercentBar.verticalCenter
                right: parent.right
            }
            visible: isPresent
            text: i18nc("Placeholder is battery percentage", "%1%", model.device.chargePercent)
        }
    }
}
