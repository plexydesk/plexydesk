add_subdirectory(plexyday)

set(sourceFiles
    plexyday.cpp
    plexydayinterface.cpp
    plexydaywidget.cpp
    cellsprovider.cpp
    cellitem.cpp
    )

set(headerFiles
    plexyday.h
    plexydayinterface.h
    plexydaywidget.h
    cellsprovider.h
    cellitem.h
    )

set(QTMOC_SRCS
    plexyday.h
    plexydayinterface.h
    plexydaywidget.h
    cellsprovider.h
    cellitem.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})


set(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

set(libs
    ${QT_QTGUI_LIBRARY}
    ${OPENGL_LIBRARIES}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTOPENGL_LIBRARY}
    )

ADD_LIBRARY(plexyday SHARED ${sourceFiles} ${QT_MOC_SRCS})

if (MINGW)
    SET_TARGET_PROPERTIES(plexyday PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
endif(MINGW)

TARGET_LINK_LIBRARIES(plexyday
    ${PLEXY_UI_CORE_LIBRARY}
    ${libs}
    )

QT5_USE_MODULES(plexyday Widgets Xml OpenGL Core )

if (NOT APPLE)
INSTALL(TARGETS plexyday DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES plexyday.desktop DESTINATION share/plexy/ext/groups)
endif (NOT APPLE)

if (APPLE)
   INSTALL(TARGETS plexyday LIBRARY DESTINATION plexydesk.app/Contents/lib/plexyext/ COMPONENT Framework)
   INSTALL(FILES plexyday.desktop DESTINATION plexydesk.app/Contents/share/plexy/ext/groups)
endif (APPLE)
