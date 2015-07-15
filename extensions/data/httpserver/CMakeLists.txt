set(sourceFiles
    httpserver.cpp
    httpserverinterface.cpp
    )

set(headerFiles
    httpserver.h
    httpserverinterface.h
    )

set(QTMOC_SRCS
    httpserver.h
    httpserverinterface.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

set(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

set(libs
    ${QT_QTCORE_LIBRARY}
    )

ADD_LIBRARY(httpserverengine SHARED ${sourceFiles} ${QT_MOC_SRCS})

if (MINGW)
    SET_TARGET_PROPERTIES(httpserverengine PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
endif(MINGW)

TARGET_LINK_LIBRARIES(httpserverengine
    ${PLEXY_UI_CORE_LIBRARY}
    ${libs}
    )

QT5_USE_MODULES(httpserverengine Widgets Xml  Core  )

if (NOT APPLE)
INSTALL(TARGETS httpserverengine DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES httpserver.desktop DESTINATION share/plexy/ext/groups)
endif (NOT APPLE)

if (APPLE)
   INSTALL(TARGETS httpserverengine LIBRARY DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_PREFIX} COMPONENT Framework)
   INSTALL(FILES httpserver.desktop DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_DATA_DIR} )
endif(APPLE)


