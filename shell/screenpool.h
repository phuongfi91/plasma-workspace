/*
 *  Copyright 2016 Marco Martin <mart@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
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

#ifndef SCREENPOOL_H
#define SCREENPOOL_H

#include <QObject>
#include <QHash>
#include <QString>
#include <QTimer>
#include <QSet>
#include <QAbstractNativeEventFilter>

#include <KConfigGroup>
#include <KSharedConfig>

class QScreen;

/*
 * This class performs three major functions:
 *   - It acts as a filter over QGuiApplication::screens() removing overlapping screens and signalling that as appropriate
 *   - It provides a consistent ID mapping with the following rules
 *   - Works around an Qt XCB backend bug
 */

class ScreenPool : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT

public:
    ScreenPool(KSharedConfig::Ptr config, QObject *parent = nullptr);
    void load();
    ~ScreenPool() override;

    QString primaryConnector() const;


    int id(const QString &connector) const;

    QString connector(int id) const;

    //all ids that are known, included screens not enabled at the moment
    QList <int> knownIds() const;

    QSet<QScreen*> screens() const;

signals:
    /*
     * A new screen has been added
     */
    void screenAdded(QScreen *added);

    /*
     * A screen is about to be removed.
     * The QScreen object is still valid at the time of this signal.
     */
    void screenRemoved(QScreen *added);

    /*
     * Signals that this IDs of two screens have swapped, and that they now represent
     * a different QScreen.
     *
     * oldPrimaryId is now at ID 0, and ID 0 is now at oldPrimaryId
     *
     */
    void primaryIdChanged(int oldPrimaryId);

protected:
    bool nativeEventFilter(const QByteArray & eventType, void * message, long * result) Q_DECL_OVERRIDE;

private:
    void onScreenAdded(QScreen *screen);
    void onScreenRemoved(QScreen *screen);
    void setPrimaryConnector(QScreen *newPrimary);

    void reconsiderOutputs();
    bool isOutputRedundant(QScreen *screen);

    void insertScreenMapping(int id, const QString &connector);
    int firstAvailableId() const;

    void save();

    KConfigGroup m_configGroup;
    QString m_primaryConnector;

    //order is important. This always contains all screens
    QMap<int, QString> m_connectorForId;
    QHash<QString, int> m_idForConnector;

    //only non-duplicate screens
    QSet<QScreen *> m_activeScreens;

    QTimer m_configSaveTimer;
};

#endif // SCREENPOOL_H
