set(CMAKE_AUTOMOC ON)

set(sourceFiles
    timezone_dialog.cpp
    timezone_dialog_plugin_impl.cpp
    )

set(headerFiles
    timezone_dialog.h
    timezone_dialog_plugin_impl.h
    )

set(QTMOC_SRCS
    timezone_dialog.h
    timezone_dialog_plugin_impl.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

set(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

set(libs
    ${QT_QTCORE_LIBRARY}
    )

ADD_LIBRARY(timezone_dialog SHARED ${sourceFiles})

if (MINGW)
    SET_TARGET_PROPERTIES(timezone_dialog PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
endif(MINGW)

TARGET_LINK_LIBRARIES(timezone_dialog
    ${PLEXY_UI_CORE_LIBRARY}
    ${libs}
    )

QT5_USE_MODULES(timezone_dialog Widgets Core)

if (NOT APPLE)
INSTALL(TARGETS timezone_dialog DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES timezone_dialog.desktop DESTINATION share/plexy/ext/groups)
endif (NOT APPLE)

if (APPLE)
   INSTALL(TARGETS timezone_dialog LIBRARY DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_PREFIX} COMPONENT Framework)
   INSTALL(FILES timezone_dialog.desktop DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_DATA_DIR} )
endif(APPLE)


