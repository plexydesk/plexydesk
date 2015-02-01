SET (sourceFiles
    desktopnotes.cpp
    desktopnotesinterface.cpp
    notewidget.cpp
    reminderwidget.cpp
    )

SET(headerFiles
    desktopnotes.h
    desktopnotesinterface.h
    notewidget.h
    reminderwidget.h
    )

SET (QTMOC_SRCS
    desktopnotes.h
    desktopnotesinterface.h
    notewidget.h
    reminderwidget.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

SET(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

SET(libs
    ${QT_QTCORE_LIBRARY}
    )

ADD_LIBRARY(desktopnoteswidget SHARED ${sourceFiles} ${QT_MOC_SRCS})

IF(MINGW)
    SET_TARGET_PROPERTIES(desktopnoteswidget PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
ENDIF(MINGW)

TARGET_LINK_LIBRARIES(desktopnoteswidget
    ${PLEXY_CORE_LIBRARY}
    ${QUETZAL_SOCIAL_KIT_LIBRARY}
    ${PLEXY_UI_CORE_LIBRARY}
    ${libs}
    )

QT5_USE_MODULES(desktopnoteswidget Widgets Xml OpenGL Core WebKitWidgets Svg)

IF (NOT APPLE)
INSTALL(TARGETS desktopnoteswidget DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES desktopnotes.desktop DESTINATION share/plexy/ext/groups)
ENDIF (NOT APPLE)

IF (APPLE)
   INSTALL(TARGETS desktopnoteswidget LIBRARY DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_PREFIX} COMPONENT Framework)
   INSTALL(FILES desktopnotes.desktop DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_DATA_DIR} )
ENDIF(APPLE)


