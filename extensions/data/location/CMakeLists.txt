set(sourceFiles
    location.cpp
    locationinterface.cpp
    )

set(headerFiles
    location.h
    locationinterface.h
    )

set(QTMOC_SRCS
    location.h
    locationinterface.h
    )

QT4_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

set(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

set(libs
    ${QT_QTCORE_LIBRARY}
    )

if (QNX)
   set(GEO_LIBS 
      bps
      )
endif(QNX)

ADD_LIBRARY(locationengine SHARED ${sourceFiles} ${QT_MOC_SRCS})

if (MINGW)
    SET_TARGET_PROPERTIES(locationengine PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
endif(MINGW)

TARGET_LINK_LIBRARIES(locationengine
    ${PLEXY_CORE_LIBRARY}
    ${libs}
    ${GEO_LIBS}
    )

INSTALL(TARGETS locationengine DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES location.desktop DESTINATION share/plexy/ext/groups)
