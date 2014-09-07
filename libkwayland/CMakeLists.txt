project(libkwayland)

include_directories(${Qt5Gui_PRIVATE_INCLUDE_DIRS})

# XXX: remove cmake/ when ECM has these macros
include(${CMAKE_SOURCE_DIR}/cmake/ECMWayland.cmake)
####

# Adjusting CMAKE_C_FLAGS to get wayland protocols to compile
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu90")

set(kwayland_LIB_SRCS
   waylandpanel.cpp
   waylandregistry.cpp
   waylandshell.cpp
   waylandsurface.cpp
)

# Protocols
ecm_add_qtwayland_client_protocol(kwayland_LIB_SRCS
    PROTOCOL ${CMAKE_CURRENT_SOURCE_DIR}/plasma-shell.xml
    BASENAME plasma-shell
    PREFIX org_kde_plasma_
)
ecm_add_qtwayland_client_protocol(kwayland_LIB_SRCS
    PROTOCOL ${CMAKE_CURRENT_SOURCE_DIR}/plasma-panel.xml
    BASENAME plasma-panel
    PREFIX org_kde_plasma_
)

add_library(kwayland STATIC ${kwayland_LIB_SRCS})
target_link_libraries(kwayland
    Qt5::Gui
    Wayland::Client
)
