set(sourceFiles
    pixabay_dialog.cpp
    pixabay_plugin_impl.cpp
    pixabay_service.cpp
    )

set(headerFiles
    pixabay_dialog.h
    pixabay_plugin_impl.h
    pixabay_service.h
    )

set(QTMOC_SRCS
    pixabay_dialog.h
    pixabay_plugin_impl.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

set(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

set(libs
    ${QT_QTCORE_LIBRARY}
    )

ADD_LIBRARY(pixabay_dialog SHARED ${sourceFiles} ${QT_MOC_SRCS})

if (MINGW)
    SET_TARGET_PROPERTIES(pixabay_dialog PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
endif(MINGW)

TARGET_LINK_LIBRARIES(pixabay_dialog
    ${PLEXY_UI_CORE_LIBRARY}
    ${libs}
    )

QT5_USE_MODULES(pixabay_dialog Widgets Core)

if (NOT APPLE)
INSTALL(TARGETS pixabay_dialog DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES pixabay_dialog.desktop DESTINATION share/plexy/ext/groups)
endif (NOT APPLE)

if (APPLE)
   INSTALL(TARGETS pixabay_dialog LIBRARY DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_PREFIX} COMPONENT Framework)
   INSTALL(FILES pixabay_dialog.desktop DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_DATA_DIR} )
endif(APPLE)


