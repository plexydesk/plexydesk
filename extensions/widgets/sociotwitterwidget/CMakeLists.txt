#ADD_SUBDIRECTORY(youtube)

set(sourceFiles
    sociotwitter.cpp
    sociotwitterinterface.cpp
    sociotwitterwidget.cpp
    )

set(headerFiles
    sociotwitter.h
    sociotwitterinterface.h
    sociotwitterck_ck_widget.h
    )

set(QTMOC_SRCS
    sociotwitter.h
    sociotwitterinterface.h
    sociotwitterck_ck_widget.h
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

ADD_LIBRARY(sociotweets SHARED ${sourceFiles} ${QT_MOC_SRCS})

TARGET_LINK_LIBRARIES(sociotweets
    qtviz
    ${libs}
    )

INSTALL(TARGETS sociotweets DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES sociotweets.desktop DESTINATION share/plexy/ext/groups)
