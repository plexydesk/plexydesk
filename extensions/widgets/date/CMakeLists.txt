set(sourceFiles
    time_event.cpp
    time_segment.cpp
    date.cpp
    dateinterface.cpp
    )

set(headerFiles
    time_event.h
    time_segment.h
    date.h
    dateinterface.h
    )

set(QTMOC_SRCS
    dateinterface.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

set(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

set(libs
    ${QT_QTCORE_LIBRARY}
    )

ADD_LIBRARY(datewidget SHARED ${sourceFiles} ${QT_MOC_SRCS})

if (MINGW)
    SET_TARGET_PROPERTIES(datewidget PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
endif(MINGW)

TARGET_LINK_LIBRARIES(datewidget
    ${PLEXY_UI_CORE_LIBRARY}
    ${libs}
    )

QT5_USE_MODULES(datewidget Widgets Xml Core)

if (NOT APPLE)
    INSTALL(TARGETS datewidget DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
    INSTALL(FILES date.desktop DESTINATION share/plexy/ext/groups)
endif (NOT APPLE)

if (APPLE)
   INSTALL(TARGETS datewidget LIBRARY DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_PREFIX} COMPONENT Framework)
   INSTALL(FILES date.desktop DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_DATA_DIR})
endif(APPLE)
