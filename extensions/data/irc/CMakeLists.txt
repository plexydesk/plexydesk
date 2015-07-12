set(sourceFiles
    ircengine.cpp
    ircengineinterface.cpp
    )

set(headerFiles
    ircengine.h
    ircengineinterface.h
    )

set(QTMOC_SRCS
    ircengine.h
    ircengineinterface.h
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
    ${QT_QTNETWORK_LIBRARY}
    ${QT_QTOPENGL_LIBRARY}
    )

ADD_LIBRARY(ircengine SHARED ${sourceFiles} ${QT_MOC_SRCS})

TARGET_LINK_LIBRARIES(ircengine
    qtviz
    ${libs}
    )

INSTALL(TARGETS ircengine DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES irc.desktop DESTINATION share/plexy/ext/groups)
