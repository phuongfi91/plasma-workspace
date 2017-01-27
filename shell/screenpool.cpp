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

#include "screenpool.h"
#include <config-plasma.h>

#include <QGuiApplication>
#include <QScreen>

#if HAVE_X11
#include <QtX11Extras/QX11Info>
#include <xcb/xcb.h>
#include <xcb/randr.h>
#include <xcb/xcb_event.h>
#endif

ScreenPool::ScreenPool(KSharedConfig::Ptr config, QObject *parent)
    : QObject(parent),
      m_configGroup(KConfigGroup(config, QStringLiteral("ScreenConnectors")))
{
    qApp->installNativeEventFilter(this);
    connect(qApp, &QGuiApplication::screenAdded, this, &ScreenPool::screenAdded);
    connect(qApp, &QGuiApplication::screenRemoved, this, &ScreenPool::screenRemoved);
    connect(qApp, &QGuiApplication::primaryScreenChanged, this, &ScreenPool::setPrimaryConnector);

    m_configSaveTimer.setSingleShot(true);
    connect(&m_configSaveTimer, &QTimer::timeout, this, [this](){
        m_configGroup.sync();
    });
}

void ScreenPool::load()
{
    m_primaryConnector = QString();
    m_connectorForId.clear();
    m_idForConnector.clear();

    QScreen *primary = qGuiApp->primaryScreen();
    if (primary) {
        m_primaryConnector = primary->name();
        if (!m_primaryConnector.isEmpty()) {
            m_connectorForId[0] = m_primaryConnector;
            m_idForConnector[m_primaryConnector] = 0;
        }
    }

    //restore the known ids to connector mappings
    foreach (const QString &key, m_configGroup.keyList()) {
        QString connector = m_configGroup.readEntry(key, QString());
        if (!key.isEmpty() && !connector.isEmpty() &&
            !m_connectorForId.contains(key.toInt()) &&
            !m_idForConnector.contains(connector)) {
            m_connectorForId[key.toInt()] = connector;
            m_idForConnector[connector] = key.toInt();
        } else if (m_idForConnector.value(connector) != key.toInt()) {
            m_configGroup.deleteEntry(key);
        }
    }

    // if there are already connected unknown screens, map those
    // all needs to be populated as soon as possible, otherwise
    // containment->screen() will return an incorrect -1
    // at startup, if it' asked before corona::addOutput()
    // is performed, driving to the creation of a new containment
    for (QScreen* screen : qGuiApp->screens()) {
        onScreenAdded(screen);
    }
}

ScreenPool::~ScreenPool()
{
    m_configGroup.sync();
}

QString ScreenPool::primaryConnector() const
{
    return m_primaryConnector;
}

void ScreenPool::onScreenAdded(QScreen *screen)
{
    const QString connector = screen->name();
    if (id(connector) < 0) {
        insertScreenMapping(firstAvailableId(), connector);
    }

    connect(screen, &QScreen::geometryChanged, this, &ScreenPool::reconsiderOutputs);

    if (!isOutputRedundant(screen)) {
        m_activeScreens.insert(screen);
        emit screenAdded(screen);
    }
}

void ScreenPool::onScreenRemoved(QScreen *screen)
{
    m_activeScreens.remove(screen);

    //we deliberately don't update any ID mapping
    emit screenRemoved(screen);

    reconsiderOutputs();
}

QSet<QScreen*> ScreenPool::screens() const
{
    return m_activeScreens;
}


bool ScreenPool::isOutputRedundant(QScreen *screen)
{
    Q_ASSERT(screen);
    const QRect geometry = screen->geometry();

    //FIXME rebase with Marco's patch
    foreach (QScreen* s, qGuiApp->screens()) {
        if (screen == s) {
            continue;
        }

        const QRect sGeometry = s->geometry();
        if (sGeometry.contains(geometry, false) &&
            sGeometry.width() > geometry.width() &&
            sGeometry.height() > geometry.height()) {
            return true;
        }
    }

    return false;
}

void ScreenPool::reconsiderOutputs()
{
    for(QScreen *screen: qApp->screens()) {
        bool wasActive = m_activeScreens.contains(screen);
        bool nowRelevant = !isOutputRedundant(screen);
        if (wasActive && !nowRelevant) {
            m_activeScreens.remove(screen);
            emit screenRemoved(screen);
        }
        else if (!wasActive && !nowRelevant) {
            m_activeScreens.insert(screen);
            emit screenAdded(screen);
        }
    }
}

void ScreenPool::setPrimaryConnector(QScreen *newPrimary)
{
    const QString primary = newPrimary->name();
    if (m_primaryConnector == primary) {
        return;
    }
    Q_ASSERT(m_idForConnector.contains(primary));

    int oldIdForNewPrimary = m_idForConnector.value(primary);

    m_idForConnector[primary] = 0;
    m_connectorForId[0] = primary;
    m_idForConnector[m_primaryConnector] = oldIdForNewPrimary;
    m_connectorForId[oldIdForNewPrimary] = m_primaryConnector;
    m_primaryConnector = primary;

    emit primaryIdChanged(oldIdForNewPrimary);

    //Since the primary screen is considered more important
    //then the others, having the primary changed may have changed what outputs are redundant and what are not
    //TODO: for a particular corner case, in which in the same moment the primary screen changes *and* geometries change to make former redundant screens to not be anymore, instead of doinf reconsiderOutputs() here, it may be better to instead put here the adding of new outputs and after the switch dance has been done, at the bottom of this function remove the eventual redundant ones

    reconsiderOutputs();

    reconsiderOutputs();
    save();
}

void ScreenPool::save()
{
    QMap<int, QString>::const_iterator i;
    for (i = m_connectorForId.constBegin(); i != m_connectorForId.constEnd(); ++i) {
        m_configGroup.writeEntry(QString::number(i.key()), i.value());
    }
    //write to disck every 30 seconds at most
    m_configSaveTimer.start(30000);
}

void ScreenPool::insertScreenMapping(int id, const QString &connector)
{
    Q_ASSERT(!m_connectorForId.contains(id) || m_connectorForId.value(id) == connector);
    Q_ASSERT(!m_idForConnector.contains(connector) || m_idForConnector.value(connector) == id);

    if (id == 0) {
        m_primaryConnector = connector;
    }

    m_connectorForId[id] = connector;
    m_idForConnector[connector] = id;
    save();
}

int ScreenPool::id(const QString &connector) const
{
    if (!m_idForConnector.contains(connector)) {
        return -1;
    }

    return m_idForConnector.value(connector);
}

QString ScreenPool::connector(int id) const
{
    Q_ASSERT(m_connectorForId.contains(id));

    return m_connectorForId.value(id);
}

int ScreenPool::firstAvailableId() const
{
    int i = 0;
    //find the first integer not stored in m_connectorForId
    //m_connectorForId is the only map, so the ids are sorted
    foreach (int existingId, m_connectorForId.keys()) {
        if (i != existingId) {
            return i;
        }
        ++i;
    }

    return i;
}

QList <int> ScreenPool::knownIds() const
{
    return m_connectorForId.keys();
}

bool ScreenPool::nativeEventFilter(const QByteArray& eventType, void* message, long int* result)
{
    Q_UNUSED(result);
#if HAVE_X11
    // a particular edge case: when we switch the only enabled screen
    // we don't have any signal about it, the primary screen changes but we have the same old QScreen* getting recycled
    // see https://bugs.kde.org/show_bug.cgi?id=373880
    // if this slot will be invoked many times, their//second time on will do nothing as name and primaryconnector will be the same by then
    if (eventType != "xcb_generic_event_t") {
        return false;
    }
    
    xcb_generic_event_t *ev = static_cast<xcb_generic_event_t *>(message);

    const auto responseType = XCB_EVENT_RESPONSE_TYPE(ev);

    const xcb_query_extension_reply_t* reply = xcb_get_extension_data(QX11Info::connection(), &xcb_randr_id);

    if (responseType == reply->first_event + XCB_RANDR_SCREEN_CHANGE_NOTIFY) {
        if (qGuiApp->primaryScreen()->name() != primaryConnector()) {
            //new screen?
            if (id(qGuiApp->primaryScreen()->name()) < 0) {
                insertScreenMapping(firstAvailableId(), qGuiApp->primaryScreen()->name());
            }
            //switch the primary screen in the pool
            setPrimaryConnector(qGuiApp->primaryScreen());
        }
    }
#endif
    return false;
}


#include "moc_screenpool.cpp"

