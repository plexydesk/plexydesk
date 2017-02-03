set(CMAKE_AUTOMOC ON)

set(sourceFiles
    desktop_settings_dialog.cpp
    desktop_settings_plugin_impl.cpp
    )

set(headerFiles
    desktop_settings_dialog.h
    desktop_settings_plugin_impl.h
    )

set(QTMOC_SRCS
    desktop_settings_dialog.h
    desktop_settings_plugin_impl.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

set(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

set(libs
    ${QT_QTCORE_LIBRARY}
    )

ADD_LIBRARY(desktop_settings_dialog SHARED ${sourceFiles})

if (MINGW)
    SET_TARGET_PROPERTIES(desktop_settings_dialog PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
endif(MINGW)

TARGET_LINK_LIBRARIES(desktop_settings_dialog
    ${PLEXY_UI_CORE_LIBRARY}
    ${QUETZAL_SOCIAL_KIT_LIBRARY}
    ${libs}
    )

#QT5_USE_MODULES(desktop_settings_dialog Widgets)

if (NOT APPLE)
    INSTALL(TARGETS desktop_settings_dialog DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
    INSTALL(FILES desktop_settings_dialog.desktop DESTINATION share/plexy/ext/groups)
endif (NOT APPLE)

if (APPLE)
   INSTALL(TARGETS desktop_settings_dialog LIBRARY DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_PREFIX} COMPONENT Framework)
   INSTALL(FILES desktop_settings_dialog.desktop DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_DATA_DIR} )
endif(APPLE)
