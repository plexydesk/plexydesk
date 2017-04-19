set(CMAKE_AUTOMOC ON)

set(sourceFiles
    style_dialog.cpp
    style_plugin_impl.cpp
    )

set(headerFiles
    style_dialog.h
    style_plugin_impl.h
    )

set(QTMOC_SRCS
    style_dialog.h
    style_plugin_impl.h
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

ADD_LIBRARY(style_dialog SHARED ${sourceFiles})

if (MINGW)
    SET_TARGET_PROPERTIES(style_dialog PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
endif(MINGW)

TARGET_LINK_LIBRARIES(style_dialog
    ${PLEXY_UI_CORE_LIBRARY}
    ${libs}
    )

if (ENABLE_QT5)
QT5_USE_MODULES(style_dialog Widgets Core)
endif()

if (NOT APPLE)
INSTALL(TARGETS style_dialog DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES style_dialog.desktop DESTINATION share/plexy/ext/groups)
endif (NOT APPLE)

if (APPLE)
   INSTALL(TARGETS style_dialog LIBRARY DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_PREFIX} COMPONENT Framework)
   INSTALL(FILES style_dialog.desktop DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_DATA_DIR} )
endif(APPLE)


