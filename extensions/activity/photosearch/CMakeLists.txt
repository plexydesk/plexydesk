set(sourceFiles
    photosearch.cpp
    photosearchinterface.cpp
    )

set(headerFiles
    photosearch.h
    photosearchinterface.h
    )

set(QTMOC_SRCS
    photosearch.h
    photosearchinterface.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

set(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

set(libs
    ${QT_QTCORE_LIBRARY}
    )

ADD_LIBRARY(desktop_settings_dialogs SHARED ${sourceFiles} ${QT_MOC_SRCS})

if (MINGW)
    SET_TARGET_PROPERTIES(desktop_settings_dialogs PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
endif(MINGW)

TARGET_LINK_LIBRARIES(desktop_settings_dialogs
    ${PLEXY_UI_CORE_LIBRARY}
    ${QUETZAL_SOCIAL_KIT_LIBRARY}
    ${libs}
    )

#QT5_USE_MODULES(desktop_settings_dialogs Widgets)

if (NOT APPLE)
    INSTALL(TARGETS desktop_settings_dialogs DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
    INSTALL(FILES photosearch.desktop DESTINATION share/plexy/ext/groups)
endif (NOT APPLE)

if (APPLE)
   INSTALL(TARGETS desktop_settings_dialogs LIBRARY DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_PREFIX} COMPONENT Framework)
   INSTALL(FILES photosearch.desktop DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_DATA_DIR} )
endif(APPLE)
