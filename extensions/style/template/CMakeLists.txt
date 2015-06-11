SET (sourceFiles
    template.cpp
    templateinterface.cpp
    )

SET(headerFiles
    template.h
    templateinterface.h
    )

SET (QTMOC_SRCS
    template.h
    templateinterface.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

SET(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

SET(libs
    ${QT_QTCORE_LIBRARY}
    )

ADD_LIBRARY(templatestyle SHARED ${sourceFiles} ${QT_MOC_SRCS})

IF(MINGW)
    SET_TARGET_PROPERTIES(templatestyle PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
ENDIF(MINGW)

TARGET_LINK_LIBRARIES(templatestyle
    ${PLEXY_CORE_LIBRARY}
    ${libs}
    )

QT5_USE_MODULES(templatestyle Widgets Xml OpenGL Core   Declarative)

IF (NOT APPLE)
INSTALL(TARGETS templatestyle DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES template.desktop DESTINATION share/plexy/ext/groups)
ENDIF (NOT APPLE)

IF (APPLE)
   INSTALL(TARGETS templatestyle LIBRARY DESTINATION plexydesk.app/Contents/lib/plexyext/ COMPONENT Framework)
   INSTALL(FILES template.desktop DESTINATION plexydesk.app/Contents/share/plexy/ext/groups)
ENDIF(APPLE)


