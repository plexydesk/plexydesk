set(CMAKE_AUTOMOC ON)

add_subdirectory(clock)

set(clocksourceFiles
    clock.cpp
    clockinterface.cpp
    )

set(headerFiles
    clock.h
    clockinterface.h
    )

set(sourceFiles
    ${clocksourceFiles}
    ${QT_MOC_SRCS}
    )

set(libs
    ${PLEXY_UI_CORE_LIBRARY}
    )

ADD_LIBRARY(plexyclock SHARED ${sourceFiles})

if (MINGW)
    SET_TARGET_PROPERTIES(plexyclock PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
endif(MINGW)

TARGET_LINK_LIBRARIES(plexyclock
    ${libs}
    )
QT5_USE_MODULES(plexyclock Widgets Xml  Core  )

if (NOT APPLE)
INSTALL(TARGETS plexyclock DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES clock.desktop DESTINATION share/plexy/ext/groups)
endif (NOT APPLE)

if (APPLE)
   INSTALL(TARGETS plexyclock LIBRARY DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_PREFIX} COMPONENT Framework)
   INSTALL(FILES clock.desktop DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_DATA_DIR} )
endif(APPLE)
