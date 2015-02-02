SET (sourceFiles
    desktopclock.cpp
    desktopclockinterface.cpp
    clockwidget.cpp
    timezone_model.cpp
    )

SET(headerFiles
    desktopclock.h
    desktopclockinterface.h
    clockwidget.h
    timezone_model.h
    )

SET (QTMOC_SRCS
    desktopclock.h
    desktopclockinterface.h
    clockwidget.h
    timezone_model.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

SET(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

SET(libs
    ${QT_QTCORE_LIBRARY}
    )

ADD_LIBRARY(desktopclock SHARED ${sourceFiles} ${QT_MOC_SRCS})

IF(MINGW)
    SET_TARGET_PROPERTIES(desktopclock PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
ENDIF(MINGW)

TARGET_LINK_LIBRARIES(desktopclock
    ${PLEXY_UI_CORE_LIBRARY}
    ${libs}
    )

QT5_USE_MODULES(desktopclock Widgets Core)

IF (NOT APPLE)
INSTALL(TARGETS desktopclock DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES desktopclock.desktop DESTINATION share/plexy/ext/groups)
ENDIF (NOT APPLE)

IF (APPLE)
   INSTALL(TARGETS desktopclock LIBRARY DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_PREFIX} COMPONENT Framework)
   INSTALL(FILES desktopclock.desktop DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_DATA_DIR} )
ENDIF(APPLE)


