SET (sourceFiles
    timezone.cpp
    timezoneinterface.cpp
    )

SET(headerFiles
    timezone.h
    timezoneinterface.h
    )

SET (QTMOC_SRCS
    timezone.h
    timezoneinterface.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

SET(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

SET(libs
    ${QT_QTCORE_LIBRARY}
    )

ADD_LIBRARY(timezone SHARED ${sourceFiles} ${QT_MOC_SRCS})

IF(MINGW)
    SET_TARGET_PROPERTIES(timezone PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
ENDIF(MINGW)

TARGET_LINK_LIBRARIES(timezone
    ${PLEXY_CORE_LIBRARY}
    ${PLEXY_UI_CORE_LIBRARY}
    ${libs}
    )

QT5_USE_MODULES(timezone Widgets Core)

IF (NOT APPLE)
INSTALL(TARGETS timezone DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES timezone.desktop DESTINATION share/plexy/ext/groups)
ENDIF (NOT APPLE)

IF (APPLE)
   INSTALL(TARGETS timezone LIBRARY DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_PREFIX} COMPONENT Framework)
   INSTALL(FILES timezone.desktop DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_DATA_DIR} )
ENDIF(APPLE)


