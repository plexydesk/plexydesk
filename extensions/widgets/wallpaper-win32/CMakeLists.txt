set(sourceFiles
    wallpaper.cpp
    wallpaperwidget.cpp
    wallpaperinterface.cpp
    )

set(headerFiles
    wallpaper.h
    wallpaperwidget.h
    wallpaperinterface.h
    )

SOURCE_GROUP(source
    FILES
    ${sourceFile}
    )

SOURCE_GROUP(headers
    FILES
    ${headerFiles}
    )

set(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

set(QTMOC_SRCS
    wallpaper.h
    wallpaperwidget.h
    wallpaperinterface.h
    )

QT4_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

set(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

ADD_LIBRARY(wallpaperchange SHARED ${sourceFiles} ${QT_MOC_SRCS})

if (MINGW)
    SET_TARGET_PROPERTIES(wallpaperchange PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
endif(MINGW)

set(libs
    ${QT_QTGUI_LIBRARY}
    ${OPENGL_LIBRARIES}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTOPENGL_LIBRARY}
    )

TARGET_LINK_LIBRARIES(wallpaperchange
    ${PLEXY_CORE_LIBRARY}
#    qtviz
    ${libs}
    )

INSTALL(TARGETS wallpaperchange DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES wallpaperchange.desktop DESTINATION share/plexy/ext/groups)
