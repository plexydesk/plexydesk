set(sourceFiles
    dock.cpp
    dockinterface.cpp
    vertical_dock.cpp
    )

set(headerFiles
    dock.h
    dockinterface.h
    vertical_dock.h
    )

set(QTMOC_SRCS
    dock.h
    dockinterface.h
    vertical_dock.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

set(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

set(libs
    ${PLEXY_UI_CORE_LIBRARY}
    ${QT_QTCORE_LIBRARY}
    )

ADD_LIBRARY(dockwidget SHARED ${sourceFiles} ${QT_MOC_SRCS})

if (MINGW)
    SET_TARGET_PROPERTIES(dockwidget PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
endif(MINGW)

TARGET_LINK_LIBRARIES(dockwidget
    ${libs}
    )

QT5_USE_MODULES(dockwidget Widgets Xml Gui Core)

if (NOT APPLE)
INSTALL(TARGETS dockwidget DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES dock.desktop DESTINATION share/plexy/ext/groups)
endif (NOT APPLE)

if (APPLE)
   INSTALL(TARGETS dockwidget LIBRARY DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_PREFIX} COMPONENT Framework)
   INSTALL(FILES dock.desktop DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_DATA_DIR} )
endif(APPLE)


