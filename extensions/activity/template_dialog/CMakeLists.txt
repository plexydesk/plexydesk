set(CMAKE_AUTOMOC ON)

set(sourceFiles
    template_dialog.cpp
    template_plugin_impl.cpp
    )

set(headerFiles
    template_dialog.h
    template_plugin_impl.h
    )

set(QTMOC_SRCS
    template_dialog.h
    template_plugin_impl.h
    )

if (ENABLE_QT5)
	#QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})
endif()

set(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

set(libs
    ${QT_QTCORE_LIBRARY}
    )

ADD_LIBRARY(template_dialog SHARED ${sourceFiles})

if (MINGW)
    SET_TARGET_PROPERTIES(template_dialog PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
endif(MINGW)

TARGET_LINK_LIBRARIES(template_dialog
    ${PLEXY_UI_CORE_LIBRARY}
    ${libs}
    )

if (ENABLE_QT5)
QT5_USE_MODULES(template_dialog Widgets Core)
endif()

if (NOT APPLE)
INSTALL(TARGETS template_dialog DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES template_dialog.desktop DESTINATION share/plexy/ext/groups)
endif (NOT APPLE)

if (APPLE)
   INSTALL(TARGETS template_dialog LIBRARY DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_PREFIX} COMPONENT Framework)
   INSTALL(FILES template_dialog.desktop DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_DATA_DIR} )
endif(APPLE)


