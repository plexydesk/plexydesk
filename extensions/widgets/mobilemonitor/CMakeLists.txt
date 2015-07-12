
if (QNX)
set(bbsource blackberryplugin.cpp)
set(bbheaders blackberryplugin.h)
endif(QNX)

set(sourceFiles
    interface.cpp
    mobilemonplugin.cpp
    plexynowwidget.cpp
    notebookbase.cpp
    note.cpp
    ${bbsource}
    )

set(headerFiles
    interface.h
    mobilemonplugin.h
    plexynowwidget.h
    notebookbase.h
    note.h
    ${bbheaders}
    )

set(QTMOC_SRCS
    interface.h
    mobilemonplugin.h
    plexynowwidget.h
    notebookbase.h
    note.h
    ${bbheaders}
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

ADD_LIBRARY(mobilemon SHARED ${sourceFiles} ${QT_MOC_SRCS})

TARGET_LINK_LIBRARIES(mobilemon
    ${PLEXY_UI_CORE_LIBRARY}
    ${libs}
    )

QT5_USE_MODULES(mobilemon Widgets Xml OpenGL Core   )

if (NOT APPLE)
INSTALL(TARGETS mobilemon DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES mobilemon.desktop DESTINATION share/plexy/ext/groups)
endif (NOT APPLE)

if (APPLE)
   INSTALL(TARGETS mobilemon LIBRARY DESTINATION plexydesk.app/Contents/lib/plexyext/ COMPONENT Framework)
   INSTALL(FILES mobilemon.desktop DESTINATION plexydesk.app/Contents/share/plexy/ext/groups)
endif (APPLE)
