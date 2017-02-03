set (CMAKE_AUTOMOC ON)

set(sourceFiles
    flickrphotosearch.cpp
    flickrphotosearchinterface.cpp
    )

set(headerFiles
    flickrphotosearch.h
    flickrphotosearchinterface.h
    )

set(QTMOC_SRCS
    flickrphotosearch.h
    flickrphotosearchinterface.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

set(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

set(libs
    ${QT_QTCORE_LIBRARY}
    ${QUETZAL_SOCIAL_KIT_LIBRARY}
    )

ADD_LIBRARY(flickrphotosearchengine SHARED ${sourceFiles})

if (MINGW)
    SET_TARGET_PROPERTIES(flickrphotosearchengine PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
endif(MINGW)

TARGET_LINK_LIBRARIES(flickrphotosearchengine
    ${PLEXY_UI_CORE_LIBRARY}
    ${libs}
    )

QT5_USE_MODULES(flickrphotosearchengine Widgets Xml  Core  )

if (NOT APPLE)
INSTALL(TARGETS flickrphotosearchengine DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES flickrphotosearch.desktop DESTINATION share/plexy/ext/groups)
endif (NOT APPLE)

if (APPLE)
   INSTALL(TARGETS flickrphotosearchengine LIBRARY DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_PREFIX} COMPONENT Framework)
   INSTALL(FILES flickrphotosearch.desktop DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_DATA_DIR} )
endif(APPLE)


