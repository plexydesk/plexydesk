SET (sourceFiles
    ubuntu.cpp
    ubuntuinterface.cpp
    )

SET(headerFiles
    ubuntu.h
    ubuntuinterface.h
    )

SET (QTMOC_SRCS
    ubuntu.h
    ubuntuinterface.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

SET(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

SET(libs
    ${QT_QTCORE_LIBRARY}
    )

ADD_LIBRARY(ubuntustyle SHARED ${sourceFiles} ${QT_MOC_SRCS})

IF(MINGW)
    SET_TARGET_PROPERTIES(ubuntustyle PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
ENDIF(MINGW)

TARGET_LINK_LIBRARIES(ubuntustyle
    ${PLEXY_CORE_LIBRARY}
    ${libs}
    )

QT5_USE_MODULES(ubuntustyle Widgets Xml OpenGL Core WebKitWidgets Svg)

IF (NOT APPLE)
INSTALL(TARGETS ubuntustyle DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES ubuntu.desktop DESTINATION share/plexy/ext/groups)
ENDIF (NOT APPLE)

IF (APPLE)
   INSTALL(TARGETS ubuntustyle LIBRARY DESTINATION plexydesk.app/Contents/lib/plexyext/ COMPONENT Framework)
   INSTALL(FILES ubuntu.desktop DESTINATION plexydesk.app/Contents/share/plexy/ext/groups)
ENDIF(APPLE)


