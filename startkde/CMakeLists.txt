add_subdirectory(kcminit)
add_subdirectory(kstartupconfig)
add_subdirectory(ksyncdbusenv)

#FIXME: reconsider, looks fishy
if(NOT CMAKE_INSTALL_PREFIX STREQUAL "/usr")
    set(EXPORT_XCURSOR_PATH "XCURSOR_PATH=${CMAKE_INSTALL_PREFIX}/share/icons:$XCURSOR_PATH\":~/.icons:/usr/share/icons:/usr/share/pixmaps:/usr/X11R6/lib/X11/icons\"; export XCURSOR_PATH")
endif()

configure_file(startkde.cmake ${CMAKE_CURRENT_BINARY_DIR}/startkde  @ONLY)
configure_file(startplasmacompositor.cmake ${CMAKE_CURRENT_BINARY_DIR}/startplasmacompositor  @ONLY)
configure_file(startplasma.cmake ${CMAKE_CURRENT_BINARY_DIR}/startplasma  @ONLY)

# Configure specific entries on startkde.d
configure_file(startkde.d/40-xdg-data-dirs.cmake ${CMAKE_CURRENT_BINARY_DIR}/startkde.d/40-xdg-data-dirs.sh  @ONLY)
configure_file(startkde.d/52-dbus-session-server.cmake ${CMAKE_CURRENT_BINARY_DIR}/startkde.d/52-dbus-session-server.sh  @ONLY)
configure_file(startkde.d/53-kdeinit-ld-bind.cmake ${CMAKE_CURRENT_BINARY_DIR}/startkde.d/53-kdeinit-ld-bind.cmake  @ONLY)

if(NOT WIN32)
  install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/startkde DESTINATION ${KDE_INSTALL_BINDIR})
  install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/startplasmacompositor DESTINATION ${KDE_INSTALL_BINDIR})
  install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/startplasma DESTINATION ${KDE_INSTALL_LIBEXECDIR})
  install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/startkde.d DESTINATION /etc PATTERN "*.cmake" EXCLUDE)
  install(FILE ${CMAKE_CURRENT_BINARY_DIR}/startkde.d/40-xdg-data-dirs.sh DESTINATOIN /etc/startkde.d/)
  install(FILE ${CMAKE_CURRENT_BINARY_DIR}/startkde.d/52-dbus-session-server.cmake DESTINATOIN /etc/startkde.d/)
  install(FILE ${CMAKE_CURRENT_BINARY_DIR}/startkde.d/53-kdeinit-ld-bind.cmake DESTINATOIN /etc/startkde.d/)
endif()
