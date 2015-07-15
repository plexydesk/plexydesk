set(sourceFiles
    openweathermaps.cpp
    openweathermapsinterface.cpp
    )

set(headerFiles
    openweathermaps.h
    openweathermapsinterface.h
    )

set(QTMOC_SRCS
    openweathermaps.h
    openweathermapsinterface.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

set(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

set(libs
    ${QT_QTCORE_LIBRARY}
    jsoncpp
    )

ADD_LIBRARY(openweathermapsengine SHARED ${sourceFiles} ${QT_MOC_SRCS})

if (MINGW)
    SET_TARGET_PROPERTIES(openweathermapsengine PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
endif(MINGW)

TARGET_LINK_LIBRARIES(openweathermapsengine
    ${PLEXY_CORE_LIBRARY}
    ${libs}
    )

QT5_USE_MODULES(openweathermapsengine Widgets Xml  Core   )

if (NOT APPLE)
INSTALL(TARGETS openweathermapsengine DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES openweathermaps.desktop DESTINATION share/plexy/ext/groups)
endif(NOT APPLE)

if (APPLE)
   INSTALL(TARGETS openweathermapsengine LIBRARY DESTINATION plexydesk.app/Contents/lib/plexyext/ COMPONENT Framework)
   INSTALL(FILES openweathermaps.desktop DESTINATION plexydesk.app/Contents/share/plexy/ext/groups)
endif(APPLE)
