SET (sourceFiles
    googleplacesengine.cpp
    googleplacesengineinterface.cpp
    )

SET(headerFiles
    googleplacesengine.h
    googleplacesengineinterface.h
    )

SET (QTMOC_SRCS
    googleplacesengine.h
    googleplacesengineinterface.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

SET(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

SET(libs
    ${QT_QTCORE_LIBRARY}
    )

ADD_LIBRARY(googleplacesengineengine SHARED ${sourceFiles} ${QT_MOC_SRCS})

IF(MINGW)
    SET_TARGET_PROPERTIES(googleplacesengineengine PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
ENDIF(MINGW)

TARGET_LINK_LIBRARIES(googleplacesengineengine
    ${PLEXY_CORE_LIBRARY}
    ${libs}
    )

QT5_USE_MODULES(googleplacesengineengine Widgets Xml OpenGL Core  )

IF (NOT APPLE)
INSTALL(TARGETS googleplacesengineengine DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES googleplacesengine.desktop DESTINATION share/plexy/ext/groups)
ENDIF(NOT APPLE)

IF (APPLE)
   INSTALL(TARGETS googleplacesengineengine LIBRARY DESTINATION plexydesk.app/Contents/lib/plexyext/ COMPONENT Framework)
   INSTALL(FILES googleplacesengine.desktop DESTINATION plexydesk.app/Contents/share/plexy/ext/groups)
ENDIF(APPLE)
