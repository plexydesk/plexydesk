if (WIN32)
    INCLUDE_DIRECTORIES(
           ${CMAKE_BINARY_DIR}/3rdparty/win32/ffmpeg/include
           )
endif(WIN32)

set(sourceFiles
    video.cpp
    ivideointerface.cpp
    vplayer.cpp
    )

set(headerFiles
    ivideointerface.h
    video.h
    vplayer.h
    )

set(QTMOC_SRCS
    ivideointerface.h
    video.h
    vplayer.h
    )

QT4_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

set(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

ADD_LIBRARY (videoengine SHARED ${sourceFiles} ${QT_MOC_SRCS})

if (WIN32)
    TARGET_LINK_LIBRARIES(videoengine
        qtviz
        ${CMAKE_SOURCE_DIR}/3rdparty/win32/ffmpeg/lib/avutil-49
        ${CMAKE_SOURCE_DIR}/3rdparty/win32/ffmpeg/lib/avformat-51
        ${CMAKE_SOURCE_DIR}/3rdparty/win32/ffmpeg/lib/avcodec-51
        ${QT_QTGUI_LIBRARY}
        ${OPENGL_LIBRARIES}
        ${QT_QTCORE_LIBRARY}
        ${QT_QTNETWORK_LIBRARY}
        ${QT_QTOPENGL_LIBRARY}
        )
ELSE(WIN32)
    TARGET_LINK_LIBRARIES(videoengine
        qtviz
        avutil
        avformat
        avcodec
        ${QT_QTGUI_LIBRARY}
        ${OPENGL_LIBRARIES}
        ${QT_QTCORE_LIBRARY}
        ${QT_QTNETWORK_LIBRARY}
        ${QT_QTOPENGL_LIBRARY}
        )
endif(WIN32)

INSTALL(TARGETS videoengine DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES videoengine.desktop DESTINATION share/plexy/ext/groups)
