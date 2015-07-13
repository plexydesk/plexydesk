add_subdirectory(youtube)

set(sourceFiles
    videoentity.cpp
    youtube.cpp
    youtubeinterface.cpp
    youtubewidget.cpp
    )

set(headerFiles
    videoentity.h
    youtube.h
    youtubeinterface.h
    youtubeck_ck_widget.h
    config.h
    )

set(QTMOC_SRCS
    youtube.h
    youtubeinterface.h
    youtubeck_ck_widget.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

set(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

ADD_LIBRARY(plexyyoutube SHARED ${sourceFiles} ${QT_MOC_SRCS})

if (MINGW)
    SET_TARGET_PROPERTIES(plexyyoutube PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
endif(MINGW)

TARGET_LINK_LIBRARIES(plexyyoutube
    ${PLEXY_UI_CORE_LIBRARY}
    )

QT5_USE_MODULES(plexyyoutube Widgets Xml OpenGL Core   )

INSTALL(TARGETS plexyyoutube DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES youtube.desktop DESTINATION share/plexy/ext/groups)
