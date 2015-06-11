SET (sourceFiles
    photosearch.cpp
    photosearchinterface.cpp
    imagecell.cpp
    imagecelladaptor.cpp
    )

SET(headerFiles
    photosearch.h
    photosearchinterface.h
    imagecell.h
    imagecelladaptor.h
    )

SET (QTMOC_SRCS
    photosearch.h
    photosearchinterface.h
    imagecell.h
    imagecelladaptor.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

SET(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

SET(libs
    ${QT_QTCORE_LIBRARY}
    )

ADD_LIBRARY(photosearchactivity SHARED ${sourceFiles} ${QT_MOC_SRCS})

IF(MINGW)
    SET_TARGET_PROPERTIES(photosearchactivity PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
ENDIF(MINGW)

TARGET_LINK_LIBRARIES(photosearchactivity
    ${PLEXY_UI_CORE_LIBRARY}
    ${QUETZAL_SOCIAL_KIT_LIBRARY}
    ${libs}
    )

QT5_USE_MODULES(photosearchactivity Widgets Xml OpenGL Core WebKitWidgets )

IF (NOT APPLE)
    INSTALL(TARGETS photosearchactivity DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
    INSTALL(FILES photosearch.desktop DESTINATION share/plexy/ext/groups)
ENDIF (NOT APPLE)

IF (APPLE)
   INSTALL(TARGETS photosearchactivity LIBRARY DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_PREFIX} COMPONENT Framework)
   INSTALL(FILES photosearch.desktop DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_DATA_DIR} )
ENDIF(APPLE)
