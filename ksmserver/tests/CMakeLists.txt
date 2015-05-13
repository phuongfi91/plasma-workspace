include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/.. )

set(testsh_SRCS
    test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../shutdowndlg.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/../logouteffect.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/../curtaineffect.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/../fadeeffect.cpp)
)

add_executable(testsh ${testsh_SRCS})

target_link_libraries(testsh
     ${SCREEN_LOCKER}
    PW::KWorkspace
    KF5::XmlGui
    KF5::GlobalAccel
    KF5::KIOCore
    KF5::KIOWidgets
    KF5::Notifications
    ${X11_LIBRARIES}
    ${X11_Xrender_LIB}
    Qt5::X11Extras
    KF5::SolidPower
    Qt5::Quick
    KF5::Declarative
    KF5::DBusAddons
    KF5::Plasma
    KF5::PlasmaQuick
    KF5::IconThemes
    KF5::I18n
)
