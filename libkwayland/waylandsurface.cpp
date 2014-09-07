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

#include <QtGui/QGuiApplication>
#include <QtGui/qpa/qplatformnativeinterface.h>
#include <QtGui/QWindow>
#include <QtGui/QScreen>

#include "waylandregistry.h"
#include "waylandshell.h"
#include "waylandsurface.h"

WaylandSurface::WaylandSurface(QWindow *window)
    : QtWayland::org_kde_plasma_surface()
    , m_window(window)
{
    // Make sure the compositor won't create a traditional shell surface
    window->setFlags(window->flags() | Qt::BypassWindowManagerHint);
    window->winId();

    // Platform native interface
    m_native = QGuiApplication::platformNativeInterface();
    if (!m_native)
        qFatal("Platform native interface not found, aborting...");

    // Get the Wayland surface for this window
    m_wlSurface = static_cast<wl_surface *>(
                m_native->nativeResourceForWindow("surface", m_window));
    if (!m_wlSurface)
        qFatal("Unable to get wl_surface from window, aborting...");

    // Create Plasma surface
    m_surface = WaylandRegistry::shell()->get_surface(m_wlSurface);
    init(m_surface);

    // Assign output everytime it changes
    connect(window, &QWindow::screenChanged, [=](QScreen *) {
        assignOutput();
    });
}

void WaylandSurface::assignOutput()
{
    if (!isInitialized())
        qFatal("Please initialize WaylandSurface before using it, aborting...");

    wl_output *wlOutput = static_cast<wl_output *>(
                m_native->nativeResourceForScreen("output", m_window->screen()));
    if (!wlOutput)
        qFatal("Unable to get wl_output from window screen, aborting...");

    set_output(wlOutput);
}

void WaylandSurface::move(const QPointF &pos)
{
    if (!isInitialized())
        qFatal("Please initialize WaylandSurface before using it, aborting...");

    set_position(pos.x(), pos.y());
}

void WaylandSurface::setRole(const Role &role)
{
    if (!isInitialized())
        qFatal("Please initialize WaylandSurface before using it, aborting...");

    set_role(static_cast<uint32_t>(role));
}
