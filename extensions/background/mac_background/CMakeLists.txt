set(sourceFiles
    mac_background.cpp
    macbginterface.cpp
    mac_backgroundrender.cpp
    )

set(headerFiles
    mac_background.h
    macbginterface.h
    mac_backgroundrender.h
    )

set(QTMOC_SRCS
    mac_background.h
    macbginterface.h
    mac_backgroundrender.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

set(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

ADD_LIBRARY(mac_background SHARED ${sourceFiles} ${QT_MOC_SRCS})

if (MINGW)
    SET_TARGET_PROPERTIES(mac_background PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
endif(MINGW)

TARGET_LINK_LIBRARIES(mac_background
    ${PLEXY_UI_CORE_LIBRARY}
    )
QT5_USE_MODULES(mac_background Widgets Xml  Core   )

if (APPLE)
   INSTALL(TARGETS mac_background LIBRARY DESTINATION plexydesk.app/Contents/lib/plexyext/ COMPONENT Framework)
   INSTALL(FILES macdesktop.desktop DESTINATION plexydesk.app/Contents/share/plexy/ext/groups)
endif(APPLE)
