set(CMAKE_AUTOMOC ON)

set(sourceFiles
    dock.cpp
    dockinterface.cpp
    vertical_dock.cpp
    desktop_button.cpp
    )

set(headerFiles
    dock.h
    dockinterface.h
    vertical_dock.h
    desktop_button.h
    )

set(QTMOC_SRCS
    dock.h
    dockinterface.h
    vertical_dock.h
    desktop_button.h
    )

if (ENABLE_QT5)
	#  QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})
endif()

if (ENABLE_QT4)
  QT4_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})
endif()


set(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

set(libs
    ${PLEXY_UI_CORE_LIBRARY}
    ${QT_QTCORE_LIBRARY}
    )

ADD_LIBRARY(dockwidget SHARED ${sourceFiles})

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

if (ENABLE_QT5)
  QT5_USE_MODULES(dockwidget Widgets Xml Gui Core)
endif()

if (ENABLE_QT4)
  QT4_USE_MODULES(dockwidget Xml Gui Core)
endif()


if (NOT APPLE)
INSTALL(TARGETS dockwidget DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES dock.desktop DESTINATION share/plexy/ext/groups)
endif (NOT APPLE)

if (APPLE)
   INSTALL(TARGETS dockwidget LIBRARY DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_PREFIX} COMPONENT Framework)
   INSTALL(FILES dock.desktop DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_DATA_DIR} )
endif(APPLE)


