add_subdirectory(cpu)

set(plexycpusrc
    cpu.cpp
    cpuwidget.cpp
    )

set(headerFiles
    cpu.h
    cpuwidget.h
    cpuinterface.h
    )

set(QTMOC_SRCS
    cpu.h
    cpuwidget.h
    cpuinterface.h
    )

QT4_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

set(sourceFiles
    ${plexycpusrc}
    ${QT_MOC_SRCS}
    )

set(libs
    ${PLEXY_CORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${OPENGL_LIBRARIES}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTOPENGL_LIBRARY}
    )

ADD_LIBRARY(plexycpu SHARED ${sourceFiles})

TARGET_LINK_LIBRARIES(plexycpu
    ${libs}
    )

INSTALL(TARGETS plexycpu DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES cpu.desktop DESTINATION share/plexy/ext/groups)
