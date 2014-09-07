/*
 *  Copyright 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef WAYLANDSURFACE_H
#define WAYLANDSURFACE_H

#include <QtCore/QObject>

#include "qwayland-plasma-shell.h"

class QWindow;
class QPlatformNativeInterface;

class WaylandSurface : public QObject, public QtWayland::org_kde_plasma_surface
{
    Q_OBJECT
public:
    enum Role {
        NoRole = 0,
        DesktopRole,
        DashboardRole,
        ConfigRole,
        OverlayRole,
        NotificationRole,
        LockRole
    };

    explicit WaylandSurface(QWindow *window);

    void assignOutput();
    void move(const QPointF &pos);
    void setRole(const Role &role);

private:
    QWindow *m_window;
    QPlatformNativeInterface *m_native;
    wl_surface *m_wlSurface;
    ::org_kde_plasma_surface *m_surface;
};

#endif // WAYLANDSURFACE_H
