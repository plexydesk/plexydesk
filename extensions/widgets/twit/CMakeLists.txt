add_subdirectory(twit)

set(sourceFiles
    videoentity.cpp
    twit.cpp
    twitinterface.cpp
    twitwidget.cpp
    )

set(headerFiles
    videoentity.h
    twit.h
    twitinterface.h
    twitck_ck_widget.h
    )

set(QTMOC_SRCS
    twit.h
    twitinterface.h
    twitck_ck_widget.h
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

ADD_LIBRARY(plexytwit SHARED ${sourceFiles} ${QT_MOC_SRCS})

TARGET_LINK_LIBRARIES(plexytwit
    ${PLEXY_CORE_LIBRARY}
    ${libs}
    )

INSTALL(TARGETS plexytwit DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES twit.desktop DESTINATION share/plexy/ext/groups)
