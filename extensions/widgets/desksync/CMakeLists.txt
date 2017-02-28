set(CMAKE_AUTOMOC ON)

set(sourceFiles
    desksync.cpp
    desksyncinterface.cpp
    )

set(headerFiles
    desksync.h
    desksyncinterface.h
    )

set(QTMOC_SRCS
    desksync.h
    desksyncinterface.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

set(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

set(libs
    ${QT_QTCORE_LIBRARY}
    )

ADD_LIBRARY(desksyncwidget SHARED ${sourceFiles})

if (MINGW)
    SET_TARGET_PROPERTIES(desksyncwidget PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
endif(MINGW)

TARGET_LINK_LIBRARIES(desksyncwidget
    ${PLEXY_UI_CORE_LIBRARY}
    ${libs}
    )

QT5_USE_MODULES(desksyncwidget Widgets Xml Core  )

if (NOT APPLE)
    INSTALL(TARGETS desksyncwidget DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
    INSTALL(FILES desksync.desktop DESTINATION share/plexy/ext/groups)
endif (NOT APPLE)

if (APPLE)
   INSTALL(TARGETS desksyncwidget LIBRARY DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_PREFIX} COMPONENT Framework)
   INSTALL(FILES desksync.desktop DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_DATA_DIR})
endif(APPLE)
