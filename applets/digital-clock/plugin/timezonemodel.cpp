/***************************************************************************
 *   Copyright (C) 2014 Kai Uwe Broulik <kde@privat.broulik.de>            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#include "timezonemodel.h"

#include <QTimeZone>
#include <KLocalizedString>
#include <KDELibs4Support/klocale.h>
#include <KDELibs4Support/ksystemtimezone.h>
#include <KDELibs4Support/kglobal.h>

TimeZoneModel::TimeZoneModel(QObject *parent) : QAbstractListModel(parent)
{
    update();
}

TimeZoneModel::~TimeZoneModel()
{
}

int TimeZoneModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_data.count();
}

QVariant TimeZoneModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid()) {
        TimeZoneData currentData = m_data.at(index.row());

        switch(role) {
        case TimeZoneIdRole:
            return currentData.id;
        case RegionRole:
            return currentData.region;
        case CityRole:
            return currentData.city;
        case CommentRole:
            return currentData.comment;
        case CheckedRole:
            return currentData.checked;
        }
    }

    return QVariant();
}

bool TimeZoneModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || value.isNull()) {
        return false;
    }

    if (role == CheckedRole) {
        m_data[index.row()].checked = value.toBool();
        emit dataChanged(index, index);
        return true;
    }

    return false;
}

void TimeZoneModel::update()
{
    beginResetModel();
    m_data.clear();

    QStringList cities;
    QHash<QString, KTimeZone> zonesByCity;

    auto timeZoneDisplayName = [](const KTimeZone &timeZone) -> QString {
        return i18n(timeZone.name().toUtf8()).replace(QLatin1Char('_'), QLatin1Char(' '));
    };

    KTimeZones *systemTimeZones = KSystemTimeZones::timeZones();

    // add UTC to the defaults
    KTimeZone utc = KTimeZone::utc();
    cities.append(utc.name());
    zonesByCity.insert(utc.name(), utc);

    const auto timeZones = systemTimeZones->zones();
    for (auto it = timeZones.constBegin(); it != timeZones.constEnd(); ++it) {
        const KTimeZone zone = (*it);
        const QString continentCity = timeZoneDisplayName(zone);
        const int separator = continentCity.lastIndexOf(QLatin1Char('/'));

        const QString key = QString("%1|%2|%3").arg(continentCity.mid(separator + 1),
                                                    continentCity.left(separator),
                                                    zone.name());
        cities.append(key);
        zonesByCity.insert(key, zone);
    }
    cities.sort(Qt::CaseInsensitive);

    foreach(const QString &key, cities) {
        const KTimeZone timeZone = zonesByCity.value(key);
        QString comment = timeZone.comment();

        if (!comment.isEmpty()) {
            comment = i18n(comment.toUtf8());
        }

        QStringList continentCity = timeZoneDisplayName(timeZone).split(QLatin1Char('/'));

        QString countryName = KGlobal::locale()->countryCodeToName(timeZone.countryCode());
        if (countryName.isEmpty()) {
            continentCity[continentCity.count() - 1] = timeZone.countryCode();
        } else {
            continentCity[continentCity.count() - 1] = countryName;
        }

        TimeZoneData newData;
        //newData.id = // TODO
        newData.region = continentCity.join(QLatin1Char('/'));
        newData.city = timeZone.name();
        newData.comment = comment;
        m_data.append(newData);
    }

    endResetModel();
}

QHash<int, QByteArray> TimeZoneModel::roleNames() const {
    return QHash<int, QByteArray>({
        {TimeZoneIdRole, "timeZoneId"},
        {RegionRole, "region"},
        {CityRole, "city"},
        {CommentRole, "comment"},
        {CheckedRole, "checked"}
    });
}
