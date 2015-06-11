ADD_SUBDIRECTORY(plexyday)

SET(sourceFiles
    plexyday.cpp
    plexydayinterface.cpp
    plexydaywidget.cpp
    cellsprovider.cpp
    cellitem.cpp
    )

SET(headerFiles
    plexyday.h
    plexydayinterface.h
    plexydaywidget.h
    cellsprovider.h
    cellitem.h
    )

SET(QTMOC_SRCS
    plexyday.h
    plexydayinterface.h
    plexydaywidget.h
    cellsprovider.h
    cellitem.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})


SET(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

SET(libs
    ${QT_QTGUI_LIBRARY}
    ${OPENGL_LIBRARIES}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTOPENGL_LIBRARY}
    )

ADD_LIBRARY(plexyday SHARED ${sourceFiles} ${QT_MOC_SRCS})

IF(MINGW)
    SET_TARGET_PROPERTIES(plexyday PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
ENDIF(MINGW)

TARGET_LINK_LIBRARIES(plexyday
    ${PLEXY_UI_CORE_LIBRARY}
    ${libs}
    )

QT5_USE_MODULES(plexyday Widgets Xml OpenGL Core )

IF (NOT APPLE)
INSTALL(TARGETS plexyday DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES plexyday.desktop DESTINATION share/plexy/ext/groups)
ENDIF (NOT APPLE)

IF (APPLE)
   INSTALL(TARGETS plexyday LIBRARY DESTINATION plexydesk.app/Contents/lib/plexyext/ COMPONENT Framework)
   INSTALL(FILES plexyday.desktop DESTINATION plexydesk.app/Contents/share/plexy/ext/groups)
ENDIF (APPLE)
