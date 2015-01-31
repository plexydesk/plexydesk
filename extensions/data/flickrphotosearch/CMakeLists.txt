SET (sourceFiles
    flickrphotosearch.cpp
    flickrphotosearchinterface.cpp
    )

SET(headerFiles
    flickrphotosearch.h
    flickrphotosearchinterface.h
    )

SET (QTMOC_SRCS
    flickrphotosearch.h
    flickrphotosearchinterface.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

SET(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

SET(libs
    ${QT_QTCORE_LIBRARY}
    ${QUETZAL_SOCIAL_KIT_LIBRARY}
    )

ADD_LIBRARY(flickrphotosearchengine SHARED ${sourceFiles} ${QT_MOC_SRCS})

IF(MINGW)
    SET_TARGET_PROPERTIES(flickrphotosearchengine PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
ENDIF(MINGW)

TARGET_LINK_LIBRARIES(flickrphotosearchengine
    ${PLEXY_CORE_LIBRARY}
    ${libs}
    )

QT5_USE_MODULES(flickrphotosearchengine Widgets Xml OpenGL Core WebKitWidgets Svg)

IF (NOT APPLE)
INSTALL(TARGETS flickrphotosearchengine DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES flickrphotosearch.desktop DESTINATION share/plexy/ext/groups)
ENDIF (NOT APPLE)

IF (APPLE)
   INSTALL(TARGETS flickrphotosearchengine LIBRARY DESTINATION plexydesk.app/Contents/lib/plexyext/ COMPONENT Framework)
   INSTALL(FILES flickrphotosearch.desktop DESTINATION plexydesk.app/Contents/share/plexy/ext/groups)
ENDIF(APPLE)


