SET (sourceFiles
    socialauth.cpp
    socialauthinterface.cpp
    )

SET(headerFiles
    socialauth.h
    socialauthinterface.h
    )

SET (QTMOC_SRCS
    socialauth.h
    socialauthinterface.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

SET(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

SET(libs
    ${QT_QTCORE_LIBRARY}
    )

ADD_LIBRARY(socialauthactivity SHARED ${sourceFiles} ${QT_MOC_SRCS})

IF(MINGW)
    SET_TARGET_PROPERTIES(socialauthactivity PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
ENDIF(MINGW)

TARGET_LINK_LIBRARIES(socialauthactivity
    ${PLEXY_CORE_LIBRARY}
    ${PLEXY_UI_CORE_LIBRARY}
    ${libs}
    )

QT5_USE_MODULES(socialauthactivity Widgets Xml OpenGL Core WebKitWidgets Svg)

IF (NOT APPLE)
INSTALL(TARGETS socialauthactivity DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES socialauth.desktop DESTINATION share/plexy/ext/groups)
ENDIF (NOT APPLE)

IF (APPLE)
   INSTALL(TARGETS socialauthactivity LIBRARY DESTINATION plexydesk.app/Contents/lib/plexyext/ COMPONENT Framework)
   INSTALL(FILES socialauth.desktop DESTINATION plexydesk.app/Contents/share/plexy/ext/groups)
ENDIF(APPLE)


