SET (sourceFiles
    cocoa.cpp
    cocoainterface.cpp
    )

SET(headerFiles
    cocoa.h
    cocoainterface.h
    )

SET (QTMOC_SRCS
    cocoainterface.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

SET(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

SET(libs
    ${QT_QTCORE_LIBRARY}
    )

ADD_LIBRARY(cocoastyle SHARED ${sourceFiles} ${QT_MOC_SRCS})

IF(MINGW)
    SET_TARGET_PROPERTIES(cocoastyle PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
ENDIF(MINGW)

TARGET_LINK_LIBRARIES(cocoastyle
    ${PLEXY_CORE_LIBRARY}
    ${libs}
    )

QT5_USE_MODULES(cocoastyle Widgets Xml OpenGL Core WebKitWidgets Svg)

IF (NOT APPLE)
INSTALL(TARGETS cocoastyle DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES cocoa.desktop DESTINATION share/plexy/ext/groups)
ENDIF (NOT APPLE)

IF (APPLE)
   INSTALL(TARGETS cocoastyle LIBRARY DESTINATION plexydesk.app/Contents/lib/plexyext/ COMPONENT Framework)
   INSTALL(FILES cocoa.desktop DESTINATION plexydesk.app/Contents/share/plexy/ext/groups)
ENDIF(APPLE)


