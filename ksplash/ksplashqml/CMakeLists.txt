project(ksplashqml)

add_subdirectory(themes)

configure_file(config-ksplashqml.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-ksplashqml.h)

set(ksplashqml_SRCS
    main.cpp
    SplashApp.cpp
    SplashWindow.cpp
   )

add_executable(ksplashqml ${ksplashqml_SRCS})

target_link_libraries(ksplashqml
    Qt5::Quick
    Qt5::DBus
    KF5::PlasmaQuick
   )

if(Wayland_FOUND)
    target_link_libraries(ksplashqml
        kwayland
        Wayland::Client
    )
endif()

install(TARGETS ksplashqml ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES org.kde.KSplash.xml DESTINATION ${DBUS_INTERFACES_INSTALL_DIR})

