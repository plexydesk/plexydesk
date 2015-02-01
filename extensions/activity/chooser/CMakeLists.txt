SET (sourceFiles
    chooser.cpp
    chooserinterface.cpp
    chooseractiondelegate.cpp
    chooseritem.cpp
    )

SET(headerFiles
    chooser.h
    chooserinterface.h
    chooseractiondelegate.h
    chooseritem.h
    )

SET (QTMOC_SRCS
    chooser.h
    chooserinterface.h
    chooseractiondelegate.h
    chooseritem.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

SET(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

SET(libs
    ${QT_QTCORE_LIBRARY}
    )

ADD_LIBRARY(icongrid SHARED ${sourceFiles} ${QT_MOC_SRCS})

IF(MINGW)
    SET_TARGET_PROPERTIES(icongrid PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
ENDIF(MINGW)

TARGET_LINK_LIBRARIES(icongrid
    ${PLEXY_CORE_LIBRARY}
    ${PLEXY_UI_CORE_LIBRARY}
    ${libs}
    )

QT5_USE_MODULES(icongrid Widgets Xml OpenGL Core WebKitWidgets Svg)

IF (NOT APPLE)
INSTALL(TARGETS icongrid DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES chooser.desktop DESTINATION share/plexy/ext/groups)
ENDIF (NOT APPLE)

IF (APPLE)
   INSTALL(TARGETS icongrid LIBRARY DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_PREFIX} COMPONENT Framework)
   INSTALL(FILES chooser.desktop DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_DATA_DIR} )
ENDIF(APPLE)


