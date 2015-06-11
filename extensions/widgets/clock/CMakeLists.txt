set(CMAKE_AUTOMOC ON)

ADD_SUBDIRECTORY(clock)

SET(clocksourceFiles
    clock.cpp
    clockwidget.cpp
    clockinterface.cpp
    )

SET(headerFiles
    clock.h
    clockwidget.h
    clockinterface.h
    )

SET(sourceFiles
    ${clocksourceFiles}
    ${QT_MOC_SRCS}
    )

SET(libs
    ${PLEXY_UI_CORE_LIBRARY}
    )

ADD_LIBRARY(plexyclock SHARED ${sourceFiles})

IF(MINGW)
    SET_TARGET_PROPERTIES(plexyclock PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
ENDIF(MINGW)

TARGET_LINK_LIBRARIES(plexyclock
    ${libs}
    )
QT5_USE_MODULES(plexyclock Widgets Xml OpenGL Core WebKitWidgets )

IF (NOT APPLE)
INSTALL(TARGETS plexyclock DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES clock.desktop DESTINATION share/plexy/ext/groups)
ENDIF (NOT APPLE)

IF (APPLE)
   INSTALL(TARGETS plexyclock LIBRARY DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_PREFIX} COMPONENT Framework)
   INSTALL(FILES clock.desktop DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_DATA_DIR} )
ENDIF(APPLE)
