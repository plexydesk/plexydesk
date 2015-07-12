set(sourceFiles
    photosearch.cpp
    photosearchinterface.cpp
    imagecell.cpp
    imagecelladaptor.cpp
    )

set(headerFiles
    photosearch.h
    photosearchinterface.h
    imagecell.h
    imagecelladaptor.h
    )

set(QTMOC_SRCS
    photosearch.h
    photosearchinterface.h
    imagecell.h
    imagecelladaptor.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

set(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

set(libs
    ${QT_QTCORE_LIBRARY}
    )

ADD_LIBRARY(system_wallpapers SHARED ${sourceFiles} ${QT_MOC_SRCS})

if (MINGW)
    SET_TARGET_PROPERTIES(system_wallpapers PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
endif(MINGW)

TARGET_LINK_LIBRARIES(system_wallpapers
    ${PLEXY_UI_CORE_LIBRARY}
    ${QUETZAL_SOCIAL_KIT_LIBRARY}
    ${libs}
    )

QT5_USE_MODULES(system_wallpapers Widgets Xml OpenGL Core  )

if (NOT APPLE)
    INSTALL(TARGETS system_wallpapers DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
    INSTALL(FILES photosearch.desktop DESTINATION share/plexy/ext/groups)
endif (NOT APPLE)

if (APPLE)
   INSTALL(TARGETS system_wallpapers LIBRARY DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_PREFIX} COMPONENT Framework)
   INSTALL(FILES photosearch.desktop DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_DATA_DIR} )
endif(APPLE)
