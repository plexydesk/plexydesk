PROJECT(videoview)

add_subdirectory(img)

set(sourceFiles
    videoitem.cpp
    videointerface.cpp
    videoplugin.cpp
    )

set(headerFiles
    videoitem.h
    videointerface.h
    videoplugin.h
    )

set(QTMOC_SRCS
    videoitem.h
    videointerface.h
    videoplugin.h
    )

QT4_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

set(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

set(libs
    ${QT_QTGUI_LIBRARY}
    ${OPENGL_LIBRARIES}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTOPENGL_LIBRARY}
    )

ADD_LIBRARY(videoview SHARED ${sourceFiles} ${QT_MOC_SRCS})

TARGET_LINK_LIBRARIES(videoview
    ${PLEXY_CORE_LIBRARY}
    ${libs}
    )

INSTALL(TARGETS videoview DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES videoview.desktop DESTINATION share/plexy/ext/groups)
