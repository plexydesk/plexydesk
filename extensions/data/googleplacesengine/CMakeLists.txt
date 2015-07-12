set(sourceFiles
    googleplacesengine.cpp
    googleplacesengineinterface.cpp
    )

set(headerFiles
    googleplacesengine.h
    googleplacesengineinterface.h
    )

set(QTMOC_SRCS
    googleplacesengine.h
    googleplacesengineinterface.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

set(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

set(libs
    ${QT_QTCORE_LIBRARY}
    )

ADD_LIBRARY(googleplacesengineengine SHARED ${sourceFiles} ${QT_MOC_SRCS})

if (MINGW)
    SET_TARGET_PROPERTIES(googleplacesengineengine PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
endif(MINGW)

TARGET_LINK_LIBRARIES(googleplacesengineengine
    ${PLEXY_CORE_LIBRARY}
    ${libs}
    )

QT5_USE_MODULES(googleplacesengineengine Widgets Xml OpenGL Core  )

if (NOT APPLE)
INSTALL(TARGETS googleplacesengineengine DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES googleplacesengine.desktop DESTINATION share/plexy/ext/groups)
endif(NOT APPLE)

if (APPLE)
   INSTALL(TARGETS googleplacesengineengine LIBRARY DESTINATION plexydesk.app/Contents/lib/plexyext/ COMPONENT Framework)
   INSTALL(FILES googleplacesengine.desktop DESTINATION plexydesk.app/Contents/share/plexy/ext/groups)
endif(APPLE)
