set(CMAKE_AUTOMOC ON)

set(sourceFiles
    event_time.cpp
    event_date.cpp
    basic_event.cpp
    event_store.cpp
    time_event.cpp
    time_segment.cpp
    event_browser_ui.cpp
    date_controller.cpp
    dateinterface.cpp
    )

set(headerFiles
    event_time.h
    event_date.h
    basic_event.h
    event_store.h
    time_event.h
    time_segment.h
    event_browser_ui.h
    date_controller.h
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

ADD_LIBRARY(datewidget SHARED ${sourceFiles})

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

add_subdirectory(test)
