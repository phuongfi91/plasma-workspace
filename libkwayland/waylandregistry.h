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

#ifndef WAYLANDREGISTRY_H
#define WAYLANDREGISTRY_H

#include <wayland-client.h>

struct wl_registry;
struct wl_registry_listener;

class WaylandShell;

class WaylandRegistry
{
public:
    explicit WaylandRegistry();

    static WaylandRegistry *instance();

    static wl_registry *registry();
    static WaylandShell *shell();

private:
    wl_registry *m_registry;
    WaylandShell *m_shell;
    int32_t m_shellId;

    static void globalCreate(void *data, wl_registry *registry,
                             uint32_t id, const char *interface,
                             uint32_t version);
    static void globalRemove(void *data, wl_registry *registry,
                             uint32_t name);

    static const struct wl_registry_listener m_listener;
};

#endif // WAYLANDREGISTRY_H
