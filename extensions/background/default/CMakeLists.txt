SET(sourceFiles
    background_controller.cpp
    classicinterface.cpp
    classicbackgroundrender.cpp
    )

SET(headerFiles
    background_controller.h
    classicinterface.h
    classicbackgroundrender.h
    )

SET(QTMOC_SRCS
    background_controller.h
    classicinterface.h
    classicbackgroundrender.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

SET(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

ADD_LIBRARY(classicbackdrop SHARED ${sourceFiles} ${QT_MOC_SRCS})

QT5_USE_MODULES(classicbackdrop Widgets Xml OpenGL Core WebKitWidgets)

IF(MINGW)
    SET_TARGET_PROPERTIES(classicbackdrop PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
ENDIF(MINGW)

TARGET_LINK_LIBRARIES(classicbackdrop
    ${PLEXY_UI_CORE_LIBRARY}
    ${QUETZAL_SOCIAL_KIT_LIBRARY}
    )

IF (NOT APPLE)
INSTALL(TARGETS classicbackdrop DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES classic.desktop DESTINATION share/plexy/ext/groups)
ENDIF(NOT APPLE)

IF (APPLE)
   INSTALL(TARGETS classicbackdrop LIBRARY DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_PREFIX} COMPONENT Framework)
   INSTALL(FILES classic.desktop DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_DATA_DIR} )
ENDIF(APPLE)
