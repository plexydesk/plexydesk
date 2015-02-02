SET (sourceFiles
    rest.cpp
    restinterface.cpp
    )

SET(headerFiles
    rest.h
    restinterface.h
    )

SET (QTMOC_SRCS
    rest.h
    restinterface.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

SET(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

ADD_LIBRARY(restengine SHARED ${sourceFiles} ${QT_MOC_SRCS})

IF(MINGW)
    SET_TARGET_PROPERTIES(restengine PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
ENDIF(MINGW)

TARGET_LINK_LIBRARIES(restengine
    ${PLEXY_UI_CORE_LIBRARY}
    )

QT5_USE_MODULES(restengine Widgets Xml OpenGL Core WebKitWidgets Svg)

IF (NOT APPLE)
  INSTALL(TARGETS restengine DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
  INSTALL(FILES rest.desktop DESTINATION share/plexy/ext/groups)
ENDIF (NOT APPLE)

IF (APPLE)
   INSTALL(TARGETS restengine LIBRARY DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_PREFIX} COMPONENT Framework)
   INSTALL(FILES rest.desktop DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_DATA_DIR} )
ENDIF(APPLE)

