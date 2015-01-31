SET(sourceFiles
    mac_background.cpp
    macbginterface.cpp
    mac_backgroundrender.cpp
    )

SET(headerFiles
    mac_background.h
    macbginterface.h
    mac_backgroundrender.h
    )

SET(QTMOC_SRCS
    mac_background.h
    macbginterface.h
    mac_backgroundrender.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

SET(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

ADD_LIBRARY(mac_background SHARED ${sourceFiles} ${QT_MOC_SRCS})

IF(MINGW)
    SET_TARGET_PROPERTIES(mac_background PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
ENDIF(MINGW)

TARGET_LINK_LIBRARIES(mac_background
    ${PLEXY_UI_CORE_LIBRARY}
    )
QT5_USE_MODULES(mac_background Widgets Xml OpenGL Core WebKitWidgets Svg Declarative)

IF (APPLE)
   INSTALL(TARGETS mac_background LIBRARY DESTINATION plexydesk.app/Contents/lib/plexyext/ COMPONENT Framework)
   INSTALL(FILES macdesktop.desktop DESTINATION plexydesk.app/Contents/share/plexy/ext/groups)
ENDIF(APPLE)
