add_definitions(-DTRANSLATION_DOMAIN=\"plasma_containmentactions_switchdesktop\")

set(switchdesktop_SRCS
    desktop.cpp
)
ki18n_wrap_ui(switchdesktop_SRCS config.ui)

add_library(plasma_containmentactions_switchdesktop MODULE ${switchdesktop_SRCS})

kcoreaddons_desktop_to_json(plasma_containmentactions_switchdesktop plasma-containmentactions-switchdesktop.desktop)

target_link_libraries(plasma_containmentactions_switchdesktop
                        Qt5::Widgets
                        KF5::Plasma
                        KF5::WindowSystem
                        KF5::I18n
                        KF5::KIOCore
                       )

install(TARGETS plasma_containmentactions_switchdesktop DESTINATION ${KDE_INSTALL_PLUGINDIR})
install(FILES plasma-containmentactions-switchdesktop.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
