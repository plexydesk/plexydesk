SET (sourceFiles
    spacepreview.cpp
    spacepreviewinterface.cpp
    )

SET(headerFiles
    spacepreview.h
    spacepreviewinterface.h
    )

SET (QTMOC_SRCS
    spacepreview.h
    spacepreviewinterface.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

SET(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

SET(libs
    ${QT_QTCORE_LIBRARY}
    )

ADD_LIBRARY(spacepreview SHARED ${sourceFiles} ${QT_MOC_SRCS})

IF(MINGW)
    SET_TARGET_PROPERTIES(spacepreview PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
ENDIF(MINGW)

TARGET_LINK_LIBRARIES(spacepreview
    ${PLEXY_CORE_LIBRARY}
    ${PLEXY_UI_CORE_LIBRARY}
    ${libs}
    )

QT5_USE_MODULES(spacepreview Widgets Core)

IF (NOT APPLE)
INSTALL(TARGETS spacepreview DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES spacepreview.desktop DESTINATION share/plexy/ext/groups)
ENDIF (NOT APPLE)

IF (APPLE)
   INSTALL(TARGETS spacepreview LIBRARY DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_PREFIX} COMPONENT Framework)
   INSTALL(FILES spacepreview.desktop DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_DATA_DIR} )
ENDIF(APPLE)


