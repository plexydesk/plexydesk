set(CMAKE_AUTOMOC ON)

set(sourceFiles
    rest.cpp
    restinterface.cpp
    )

set(headerFiles
    rest.h
    restinterface.h
    )

set(QTMOC_SRCS
    rest.h
    restinterface.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

set(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

ADD_LIBRARY(restengine SHARED ${sourceFiles})

if (MINGW)
    SET_TARGET_PROPERTIES(restengine PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
endif(MINGW)

TARGET_LINK_LIBRARIES(restengine
    ${PLEXY_UI_CORE_LIBRARY}
    )

QT5_USE_MODULES(restengine Widgets Xml  Core  )

if (NOT APPLE)
  INSTALL(TARGETS restengine DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
  INSTALL(FILES rest.desktop DESTINATION share/plexy/ext/groups)
endif (NOT APPLE)

if (APPLE)
   INSTALL(TARGETS restengine LIBRARY DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_PREFIX} COMPONENT Framework)
   INSTALL(FILES rest.desktop DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_DATA_DIR} )
endif(APPLE)

