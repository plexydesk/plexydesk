set(sourceFiles
    desktopactivitymonitor.cpp
    desktopactivitymonitorinterface.cpp
    windowobject.cpp
    )

set(headerFiles
    desktopactivitymonitor.h
    desktopactivitymonitorinterface.h
    platformactivitymonitor.h
    windowobject.h
    )

set(QTMOC_SRCS
    desktopactivitymonitor.h
    desktopactivitymonitorinterface.h
    platformactivitymonitor.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

set(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

set(libs
    ${QUETZAL_DATA_KIT_LIBRARY}
    ${QT_QTCORE_LIBRARY}
    )

if (APPLE)
    message("Looking for Apple Specific Depedencies")

    INCLUDE_DIRECTORIES (/System/Library/Frameworks)

    FIND_LIBRARY(COCOA_LIBRARY Cocoa)
    FIND_LIBRARY(IOKIT_LIBRARY IOKit)
    FIND_LIBRARY(GLUT_LIBRARY GLUT)
    FIND_LIBRARY(QUARTZ_CORE_LIBRARY QuartzCore)
    FIND_LIBRARY(OpenGL_LIBRARY OpenGL)

    MARK_AS_ADVANCED (COCOA_LIBRARY
                      GLUT_LIBRARY
                      QUARTZ_CORE_LIBRARY
                      IOKIT_LIBRARY
                      OpenGL_LIBRARY)

    set(MAC_LIBS ${COCOA_LIBRARY} ${GLUT_LIBRARY} ${OpenGL_LIBRARY} ${IOKIT_LIBRARY} ${QUARTZ_CORE_LIBRARY})

    set(MAC_EXTRA_SRC platformactivitymonitor.mm)

endif(APPLE)

ADD_LIBRARY(desktopactivitymonitorengine SHARED ${sourceFiles} ${QT_MOC_SRCS} ${MAC_EXTRA_SRC})

if (MINGW)
    SET_TARGET_PROPERTIES(desktopactivitymonitorengine PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
endif(MINGW)

TARGET_LINK_LIBRARIES(desktopactivitymonitorengine
    ${PLEXY_CORE_LIBRARY}
    ${MAC_LIBS}
    ${libs}
    )

QT5_USE_MODULES(desktopactivitymonitorengine Widgets Xml OpenGL Core   )

if (NOT APPLE)
INSTALL(TARGETS desktopactivitymonitorengine DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES desktopactivitymonitor.desktop DESTINATION share/plexy/ext/groups)
endif (NOT APPLE)

if (APPLE)
   INSTALL(TARGETS desktopactivitymonitorengine LIBRARY DESTINATION plexydesk.app/Contents/lib/plexyext/ COMPONENT Framework)
   INSTALL(FILES desktopactivitymonitor.desktop DESTINATION plexydesk.app/Contents/share/plexy/ext/groups)
endif(APPLE)

