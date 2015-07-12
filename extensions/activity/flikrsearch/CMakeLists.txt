set(sourceFiles
    flikrsearch.cpp
    flikrsearchinterface.cpp
    photocell.cpp
    photocelladaptor.cpp
    )

set(headerFiles
    flikrsearch.h
    flikrsearchinterface.h
    photocell.h
    photocelladaptor.h
    )

set(QTMOC_SRCS
    flikrsearch.h
    flikrsearchinterface.h
    photocell.h
    photocelladaptor.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

set(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

set(libs
    ${QT_QTCORE_LIBRARY}
    )

ADD_LIBRARY(flikrsearchactivity SHARED ${sourceFiles} ${QT_MOC_SRCS})

if (MINGW)
    SET_TARGET_PROPERTIES(flikrsearchactivity PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
endif(MINGW)

TARGET_LINK_LIBRARIES(flikrsearchactivity
    ${QUETZAL_SOCIAL_KIT_LIBRARY}
    ${PLEXY_UI_CORE_LIBRARY}
    ${libs}
    )

QT5_USE_MODULES(flikrsearchactivity Widgets Xml OpenGL Core  )

if (NOT APPLE)
INSTALL(TARGETS flikrsearchactivity DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES flikrsearch.desktop DESTINATION share/plexy/ext/groups)
endif (NOT APPLE)

if (APPLE)
   INSTALL(TARGETS flikrsearchactivity LIBRARY DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_PREFIX} COMPONENT Framework)
   INSTALL(FILES flikrsearch.desktop DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_DATA_DIR} )
endif(APPLE)


