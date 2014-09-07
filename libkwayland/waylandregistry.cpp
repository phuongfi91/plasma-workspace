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

#include <QtCore/QDebug>
#include <QtGui/QGuiApplication>
#include <QtGui/qpa/qplatformnativeinterface.h>

#include "waylandregistry.h"
#include "waylandpanel.h"
#include "waylandshell.h"

#include <wayland-client.h>
#include <wayland-server.h>

Q_GLOBAL_STATIC(WaylandRegistry, s_globalRegistry)

WaylandRegistry::WaylandRegistry()
    : m_registry(Q_NULLPTR)
    , m_shell(Q_NULLPTR)
    , m_shellId(-1)
{
    QPlatformNativeInterface *native =
            QGuiApplication::platformNativeInterface();
    if (!native)
        qFatal("Platform native interface not found, aborting...");

    wl_display *display = static_cast<wl_display *>(
                native->nativeResourceForIntegration("display"));
    if (!display)
        qFatal("Wayland connection is not available, aborting...");

    m_registry = wl_display_get_registry(display);
    if (!m_registry)
        qFatal("Wayland registry unavailable, aborting...");
    wl_registry_add_listener(m_registry, &WaylandRegistry::m_listener, this);
}

WaylandRegistry *WaylandRegistry::instance()
{
    return s_globalRegistry();
}

wl_registry *WaylandRegistry::registry()
{
    return s_globalRegistry->m_registry;
}

WaylandShell *WaylandRegistry::shell()
{
    return s_globalRegistry->m_shell;
}

void WaylandRegistry::globalCreate(void *data, wl_registry *registry,
                                  uint32_t id, const char *interface,
                                  uint32_t version)
{
    Q_UNUSED(registry);

    WaylandRegistry *self = static_cast<WaylandRegistry *>(data);
    if (!self) {
        qWarning("Invalid data pointer passed to global interface creation!");
        return;
    }

    if (strcmp(interface, "org_kde_plasma_shell") == 0) {
        if (version == 1) {
            self->m_shell = new WaylandShell(id);
            self->m_shellId = static_cast<int32_t>(id);
        } else {
            qWarning("org_kde_plasma_shell is version %d we want 1",
                     version);
        }
    }
}

void WaylandRegistry::globalRemove(void *data, wl_registry *registry,
                                  uint32_t name)
{
    Q_UNUSED(registry);

    WaylandRegistry *self = static_cast<WaylandRegistry *>(data);
    if (!self) {
        qWarning() << "Invalid data pointer passed to global interface creation!";
        return;
    }

    if (self->m_shell && self->m_shellId == static_cast<int32_t>(name)) {
        delete self->m_shell;
        self->m_shell = Q_NULLPTR;
    }
}

const struct wl_registry_listener WaylandRegistry::m_listener = {
    WaylandRegistry::globalCreate,
    WaylandRegistry::globalRemove
};
