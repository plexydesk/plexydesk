set(CMAKE_AUTOMOC ON)

set(sourceFiles
    deskcontrol.cpp
    deskcontrolinterface.cpp
    )

set(headerFiles
    deskcontrol.h
    deskcontrolinterface.h
    )

set(QTMOC_SRCS
    deskcontrol.h
    deskcontrolinterface.h
    )

if (ENABLE_QT5)
QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})
endif()

set(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

set(libs
    ${QT_QTCORE_LIBRARY}
    )

ADD_LIBRARY(deskcontrolwidget SHARED ${sourceFiles})

if (MINGW)
    SET_TARGET_PROPERTIES(deskcontrolwidget PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
endif(MINGW)

TARGET_LINK_LIBRARIES(deskcontrolwidget
    ${PLEXY_UI_CORE_LIBRARY}
    ${libs}
    )

if (ENABLE_QT5)
QT5_USE_MODULES(deskcontrolwidget Widgets Xml Core  )
endif()

if (NOT APPLE)
    INSTALL(TARGETS deskcontrolwidget DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
    INSTALL(FILES deskcontrol.desktop DESTINATION share/plexy/ext/groups)
endif (NOT APPLE)

if (APPLE)
   INSTALL(TARGETS deskcontrolwidget LIBRARY DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_PREFIX} COMPONENT Framework)
   INSTALL(FILES deskcontrol.desktop DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_DATA_DIR})
endif(APPLE)
