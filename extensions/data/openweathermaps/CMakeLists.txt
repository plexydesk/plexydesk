SET (sourceFiles
    openweathermaps.cpp
    openweathermapsinterface.cpp
    )

SET(headerFiles
    openweathermaps.h
    openweathermapsinterface.h
    )

SET (QTMOC_SRCS
    openweathermaps.h
    openweathermapsinterface.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

SET(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

SET(libs
    ${QT_QTCORE_LIBRARY}
    jsoncpp
    )

ADD_LIBRARY(openweathermapsengine SHARED ${sourceFiles} ${QT_MOC_SRCS})

IF(MINGW)
    SET_TARGET_PROPERTIES(openweathermapsengine PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
ENDIF(MINGW)

TARGET_LINK_LIBRARIES(openweathermapsengine
    ${PLEXY_CORE_LIBRARY}
    ${libs}
    )

QT5_USE_MODULES(openweathermapsengine Widgets Xml OpenGL Core WebKitWidgets Svg Declarative)

IF (NOT APPLE)
INSTALL(TARGETS openweathermapsengine DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES openweathermaps.desktop DESTINATION share/plexy/ext/groups)
ENDIF(NOT APPLE)

IF (APPLE)
   INSTALL(TARGETS openweathermapsengine LIBRARY DESTINATION plexydesk.app/Contents/lib/plexyext/ COMPONENT Framework)
   INSTALL(FILES openweathermaps.desktop DESTINATION plexydesk.app/Contents/share/plexy/ext/groups)
ENDIF(APPLE)
