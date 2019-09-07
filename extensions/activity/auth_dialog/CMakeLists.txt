set(CMAKE_AUTOMOC ON)

set(sourceFiles
    auth_dialog.cpp
    auth_plugin_impl.cpp
    )

set(headerFiles
    auth_dialog.h
    auth_plugin_impl.h
    )

set(QTMOC_SRCS
    auth_dialog.h
    auth_plugin_impl.h
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

ADD_LIBRARY(auth_dialog SHARED ${sourceFiles})

if (MINGW)
    SET_TARGET_PROPERTIES(auth_dialog PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
endif(MINGW)

TARGET_LINK_LIBRARIES(auth_dialog
    ${PLEXY_UI_CORE_LIBRARY}
    ${libs}
    )

if (ENABLE_QT5)
QT5_USE_MODULES(auth_dialog Widgets Core)
endif()

if (NOT APPLE)
INSTALL(TARGETS auth_dialog DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES auth_dialog.desktop DESTINATION share/plexy/ext/groups)
endif (NOT APPLE)

if (APPLE)
   INSTALL(TARGETS auth_dialog LIBRARY DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_PREFIX} COMPONENT Framework)
   INSTALL(FILES auth_dialog.desktop DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_DATA_DIR} )
endif(APPLE)


