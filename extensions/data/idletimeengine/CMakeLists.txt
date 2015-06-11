SET (sourceFiles
    idletimeengine.cpp
    idletimeengineinterface.cpp
    idletimedetector.cpp
    )

SET(headerFiles
    idletimeengine.h
    idletimeengineinterface.h
    idletimedetector.h
    appleidletimedetector.h
    )

SET (QTMOC_SRCS
    idletimeengine.h
    idletimeengineinterface.h
    idletimedetector.h
    appleidletimedetector.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

SET(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

SET(libs
    ${QT_QTCORE_LIBRARY}
    )

IF(APPLE)
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
    SET(MAC_LIBS ${COCOA_LIBRARY} ${GLUT_LIBRARY} ${OpenGL_LIBRARY} ${IOKIT_LIBRARY} ${QUARTZ_CORE_LIBRARY})

     set(APPLE_SRC appleidletimedetector.mm)
ENDIF(APPLE)

ADD_LIBRARY(idletimeengineengine SHARED ${sourceFiles} ${QT_MOC_SRCS} ${APPLE_SRC})

IF(MINGW)
    SET_TARGET_PROPERTIES(idletimeengineengine PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
ENDIF(MINGW)

TARGET_LINK_LIBRARIES(idletimeengineengine
    ${PLEXY_CORE_LIBRARY}
    ${libs}
    ${MAC_LIBS}
    )

QT5_USE_MODULES(idletimeengineengine Widgets Xml OpenGL Core WebKitWidgets  Declarative)

IF (NOT APPLE)
INSTALL(TARGETS idletimeengineengine DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES idletimeengine.desktop DESTINATION share/plexy/ext/groups)
ENDIF (NOT APPLE)

IF (APPLE)
   INSTALL(TARGETS idletimeengineengine LIBRARY DESTINATION plexydesk.app/Contents/lib/plexyext/ COMPONENT Framework)
   INSTALL(FILES idletimeengine.desktop DESTINATION plexydesk.app/Contents/share/plexy/ext/groups)
ENDIF(APPLE)


