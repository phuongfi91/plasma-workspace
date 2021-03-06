add_definitions(-DTRANSLATION_DOMAIN=\"ksmserver\")

include_directories(${PHONON_INCLUDE_DIR})

check_library_exists(ICE _IceTransNoListen "" HAVE__ICETRANSNOLISTEN)
configure_file(config-ksmserver.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-ksmserver.h)

add_definitions("-DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII")
add_definitions(-DQT_NO_NARROWING_CONVERSIONS_IN_CONNECT)
add_definitions(-DQT_NO_URL_CAST_FROM_STRING)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_subdirectory(logout-greeter)
add_subdirectory(switchuser-greeter)
if(BUILD_TESTING)
   add_subdirectory(tests)
endif()

########### next target ###############

set(ksmserver_KDEINIT_SRCS
    main.cpp
    server.cpp
    legacy.cpp
    startup.cpp
    autostart.cpp
    shutdown.cpp
    client.cpp
)
ecm_qt_declare_logging_category(ksmserver_KDEINIT_SRCS HEADER ksmserver_debug.h IDENTIFIER KSMSERVER CATEGORY_NAME org.kde.kf5.ksmserver)

set(kcminit_adaptor ${plasma-workspace_SOURCE_DIR}/startkde/kcminit/main.h)
set(kcminit_xml ${CMAKE_CURRENT_BINARY_DIR}/org.kde.KCMinit.xml)

qt5_generate_dbus_interface( ${kcminit_adaptor} ${kcminit_xml} )
qt5_add_dbus_interface( ksmserver_KDEINIT_SRCS ${kcminit_xml} kcminit_interface )

# FIXME: This is actually not disabled any more because OrgKDEKlauncherInterface isn't provided
# otherwise.
# # This is actually now disabled, because OrgKDEKlauncherInterface is also provided
# # by kdecore, it is not autogenerated and is not binary compatible with a currently
# # generated version, thus at certain circumstances leading to strange crashes.
# # This should be fixed for KDE5.
# # KLauchner.xml is installed by kdelibs, so it is in KDE4_DBUS_INTERFACES_DIR
set(klauncher_xml ${KINIT_DBUS_INTERFACES_DIR}/kf5_org.kde.KLauncher.xml)
qt5_add_dbus_interface( ksmserver_KDEINIT_SRCS ${klauncher_xml} klauncher_interface )

qt5_add_dbus_adaptor( ksmserver_KDEINIT_SRCS org.kde.KSMServerInterface.xml server.h KSMServer )

kf5_add_kdeinit_executable( ksmserver ${ksmserver_KDEINIT_SRCS})

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KSMServerDBusInterface")

configure_package_config_file(KSMServerDBusInterfaceConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/KSMServerDBusInterfaceConfig.cmake
    PATH_VARS KDE_INSTALL_DBUSINTERFACEDIR
    INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR})

target_link_libraries(kdeinit_ksmserver
    PW::KScreenLocker
    PW::KWorkspace
    KF5::XmlGui
    KF5::GlobalAccel
    KF5::KIOCore
    KF5::KIOWidgets
    ${X11_LIBRARIES}
    ${X11_Xrender_LIB}
    Qt5::X11Extras
    KF5::Solid
    Qt5::Quick
    KF5::Declarative
    KF5::DBusAddons
    KF5::Package
    KF5::KDELibs4Support # Solid/PowerManagement
    ${PHONON_LIBRARIES}
    Qt5::Concurrent
)

install(TARGETS kdeinit_ksmserver ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(TARGETS ksmserver ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/KSMServerDBusInterfaceConfig.cmake
    DESTINATION ${CMAKECONFIG_INSTALL_DIR})
########### next target ###############

set(kcheckrunning_SRCS
    kcheckrunning.cpp)

add_executable( kcheckrunning ${kcheckrunning_SRCS})

target_link_libraries(kcheckrunning ${X11_LIBRARIES})
target_include_directories(kcheckrunning PRIVATE ${X11_X11_INCLUDE_PATH})

install(TARGETS kcheckrunning ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install( FILES org.kde.KSMServerInterface.xml DESTINATION ${KDE_INSTALL_DBUSINTERFACEDIR})
