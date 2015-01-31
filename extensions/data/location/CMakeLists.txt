SET (sourceFiles
    location.cpp
    locationinterface.cpp
    )

SET(headerFiles
    location.h
    locationinterface.h
    )

SET (QTMOC_SRCS
    location.h
    locationinterface.h
    )

QT4_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

SET(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

SET(libs
    ${QT_QTCORE_LIBRARY}
    )

IF(QNX)
   SET (GEO_LIBS 
      bps
      )
ENDIF(QNX)

ADD_LIBRARY(locationengine SHARED ${sourceFiles} ${QT_MOC_SRCS})

IF(MINGW)
    SET_TARGET_PROPERTIES(locationengine PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
ENDIF(MINGW)

TARGET_LINK_LIBRARIES(locationengine
    ${PLEXY_CORE_LIBRARY}
    ${libs}
    ${GEO_LIBS}
    )

INSTALL(TARGETS locationengine DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES location.desktop DESTINATION share/plexy/ext/groups)
