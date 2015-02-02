SET (sourceFiles
    snapframe.cpp
    dock.cpp
    dockinterface.cpp
    )

SET(headerFiles
    dock.h
    dockinterface.h
    snapframe.h
    )

SET (QTMOC_SRCS
    dock.h
    dockinterface.h
    snapframe.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

SET(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

SET(libs
    ${PLEXY_UI_CORE_LIBRARY}
    ${QT_QTCORE_LIBRARY}
    )

ADD_LIBRARY(dockwidget SHARED ${sourceFiles} ${QT_MOC_SRCS})

IF(MINGW)
    SET_TARGET_PROPERTIES(dockwidget PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
ENDIF(MINGW)

TARGET_LINK_LIBRARIES(dockwidget
    ${libs}
    )

QT5_USE_MODULES(dockwidget Widgets Xml OpenGL Core WebKitWidgets Svg)

IF (NOT APPLE)
INSTALL(TARGETS dockwidget DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES dock.desktop DESTINATION share/plexy/ext/groups)
ENDIF (NOT APPLE)

IF (APPLE)
   INSTALL(TARGETS dockwidget LIBRARY DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_PREFIX} COMPONENT Framework)
   INSTALL(FILES dock.desktop DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_DATA_DIR} )
ENDIF(APPLE)


