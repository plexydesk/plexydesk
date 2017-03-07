set(CMAKE_AUTOMOC ON)

set(sourceFiles
    progress_dialog_view.cpp
    progress_dialog_plugin_impl.cpp
    )

set(headerFiles
    progress_dialog_view.h
    progress_dialog_plugin_impl.h
    )

set(QTMOC_SRCS
    progress_dialog_view.h
    progress_dialog_plugin_impl.h
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
    ${PLEXY_UI_CORE_LIBRARY}
    )

ADD_LIBRARY(progress_dialog SHARED ${sourceFiles})

if (MINGW)
    SET_TARGET_PROPERTIES(progress_dialog PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
endif(MINGW)

TARGET_LINK_LIBRARIES(progress_dialog
    ${PLEXY_UI_CORE_LIBRARY}
    ${libs}
    )

if (ENABLE_QT5)
QT5_USE_MODULES(progress_dialog Widgets Xml  Core  )
endif()

if (NOT APPLE)
INSTALL(TARGETS progress_dialog DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES progress_dialog.desktop DESTINATION share/plexy/ext/groups)
endif (NOT APPLE)

if (APPLE)
   INSTALL(TARGETS progress_dialog LIBRARY DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_PREFIX} COMPONENT Framework)
   INSTALL(FILES progress_dialog.desktop DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_DATA_DIR} )
endif(APPLE)

