set(sourceFiles
    idletimeengine.cpp
    idletimeengineinterface.cpp
    idletimedetector.cpp
    )

set(headerFiles
    idletimeengine.h
    idletimeengineinterface.h
    idletimedetector.h
    appleidletimedetector.h
    )

set(QTMOC_SRCS
    idletimeengine.h
    idletimeengineinterface.h
    idletimedetector.h
    appleidletimedetector.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

set(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

set(libs
    ${QT_QTCORE_LIBRARY}
    )

if (APPLE)
    message("Looking for Apple Specific Depedencies")
    INCLUDE_DIRECTORIES (/System/Library/Frameworks)
    FIND_LIBRARY(COCOA_LIBRARY Cocoa)
    FIND_LIBRARY(IOKIT_LIBRARY IOKit)
    FIND_LIBRARY(GLUT_LIBRARY GLUT)
    FIND_LIBRARY(QUARTZ_CORE_LIBRARY QuartzCore)

    FIND_LIBRARY(OpenGL_LIBRARY )
    MARK_AS_ADVANCED (COCOA_LIBRARY
                      GLUT_LIBRARY
                      QUARTZ_CORE_LIBRARY
                      IOKIT_LIBRARY
                      OpenGL_LIBRARY)
    set(MAC_LIBS ${COCOA_LIBRARY} ${GLUT_LIBRARY} ${OpenGL_LIBRARY} ${IOKIT_LIBRARY} ${QUARTZ_CORE_LIBRARY})

     set(APPLE_SRC appleidletimedetector.mm)
endif(APPLE)

ADD_LIBRARY(idletimeengineengine SHARED ${sourceFiles} ${QT_MOC_SRCS} ${APPLE_SRC})

if (MINGW)
    SET_TARGET_PROPERTIES(idletimeengineengine PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
endif(MINGW)

TARGET_LINK_LIBRARIES(idletimeengineengine
    ${PLEXY_CORE_LIBRARY}
    ${libs}
    ${MAC_LIBS}
    )

QT5_USE_MODULES(idletimeengineengine Widgets Xml  Core   )

if (NOT APPLE)
INSTALL(TARGETS idletimeengineengine DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES idletimeengine.desktop DESTINATION share/plexy/ext/groups)
endif (NOT APPLE)

if (APPLE)
   INSTALL(TARGETS idletimeengineengine LIBRARY DESTINATION plexydesk.app/Contents/lib/plexyext/ COMPONENT Framework)
   INSTALL(FILES idletimeengine.desktop DESTINATION plexydesk.app/Contents/share/plexy/ext/groups)
endif(APPLE)


