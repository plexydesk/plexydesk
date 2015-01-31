SET (sourceFiles
    httpserver.cpp
    httpserverinterface.cpp
    )

SET(headerFiles
    httpserver.h
    httpserverinterface.h
    )

SET (QTMOC_SRCS
    httpserver.h
    httpserverinterface.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

SET(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

SET(libs
    ${QT_QTCORE_LIBRARY}
    )

ADD_LIBRARY(httpserverengine SHARED ${sourceFiles} ${QT_MOC_SRCS})

IF(MINGW)
    SET_TARGET_PROPERTIES(httpserverengine PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
ENDIF(MINGW)

TARGET_LINK_LIBRARIES(httpserverengine
    ${PLEXY_CORE_LIBRARY}
    ${libs}
    )

QT5_USE_MODULES(httpserverengine Widgets Xml OpenGL Core WebKitWidgets Svg)

IF (NOT APPLE)
INSTALL(TARGETS httpserverengine DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES httpserver.desktop DESTINATION share/plexy/ext/groups)
ENDIF (NOT APPLE)

IF (APPLE)
   INSTALL(TARGETS httpserverengine LIBRARY DESTINATION plexydesk.app/Contents/lib/plexyext/ COMPONENT Framework)
   INSTALL(FILES httpserver.desktop DESTINATION plexydesk.app/Contents/share/plexy/ext/groups)
ENDIF(APPLE)


