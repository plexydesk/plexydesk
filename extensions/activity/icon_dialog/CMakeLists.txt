set(CMAKE_AUTOMOC ON)

set(sourceFiles
    icon_dialog.cpp
    icon_dialog_plugin_impl.cpp
    )

set(headerFiles
    icon_dialog.h
    icon_dialog_plugin_impl.h
    )

set(QTMOC_SRCS
    icon_dialog.h
    icon_dialog_plugin_impl.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

set(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

set(libs
    ${QT_QTCORE_LIBRARY}
    )

ADD_LIBRARY(icon_dialog SHARED ${sourceFiles})

if (MINGW)
    SET_TARGET_PROPERTIES(icon_dialog PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
endif(MINGW)

TARGET_LINK_LIBRARIES(icon_dialog
    ${PLEXY_UI_CORE_LIBRARY}
    ${libs}
    )

QT5_USE_MODULES(icon_dialog Widgets Xml  Core  )

if (NOT APPLE)
INSTALL(TARGETS icon_dialog DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES icon_dialog.desktop DESTINATION share/plexy/ext/groups)
endif (NOT APPLE)

if (APPLE)
   INSTALL(TARGETS icon_dialog LIBRARY DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_PREFIX} COMPONENT Framework)
   INSTALL(FILES icon_dialog.desktop DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_DATA_DIR} )
endif(APPLE)


