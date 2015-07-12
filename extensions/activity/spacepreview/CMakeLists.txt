set(sourceFiles
    spacepreview.cpp
    spacepreviewinterface.cpp
    )

set(headerFiles
    spacepreview.h
    spacepreviewinterface.h
    )

set(QTMOC_SRCS
    spacepreview.h
    spacepreviewinterface.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

set(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

set(libs
    ${QT_QTCORE_LIBRARY}
    )

ADD_LIBRARY(spacepreview SHARED ${sourceFiles} ${QT_MOC_SRCS})

if (MINGW)
    SET_TARGET_PROPERTIES(spacepreview PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
endif(MINGW)

TARGET_LINK_LIBRARIES(spacepreview
    ${PLEXY_UI_CORE_LIBRARY}
    ${libs}
    )

QT5_USE_MODULES(spacepreview Widgets Core)

if (NOT APPLE)
INSTALL(TARGETS spacepreview DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES spacepreview.desktop DESTINATION share/plexy/ext/groups)
endif (NOT APPLE)

if (APPLE)
   INSTALL(TARGETS spacepreview LIBRARY DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_PREFIX} COMPONENT Framework)
   INSTALL(FILES spacepreview.desktop DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_DATA_DIR} )
endif(APPLE)


