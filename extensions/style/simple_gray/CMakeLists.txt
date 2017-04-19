set(CMAKE_AUTOMOC ON)

set(sourceFiles
    simple_gray.cpp
    simple_gray_impl.cpp
    )

set(headerFiles
    simple_gray.h
    simple_gray_impl.h
    )

set(QTMOC_SRCS
    simple_gray_impl.h
    )

if (ENABLE_QT5)
	QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})
endif()

if (ENABEL_QT4)
QT4_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})
endif()

set(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

set(libs
    ${QT_QTCORE_LIBRARY}
    )

ADD_LIBRARY(simplegray SHARED ${sourceFiles})

if (MINGW)
    SET_TARGET_PROPERTIES(simplegray PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
endif(MINGW)

TARGET_LINK_LIBRARIES(simplegray
    ${PLEXY_UI_CORE_LIBRARY}
    ${libs}
    )

if (ENABLE_QT5)
QT5_USE_MODULES(simplegray Widgets Xml  Core  )
endif()

if (ENABEL_QT4)
QT4_USE_MODULES(cocoastyle Gui Xml  Core  )
endif()

if (NOT APPLE)
INSTALL(TARGETS simplegray DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES simplegray.desktop DESTINATION share/plexy/ext/groups)
endif (NOT APPLE)

if (APPLE)
   INSTALL(TARGETS simplegray LIBRARY DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_PREFIX} COMPONENT Framework)
   INSTALL(FILES simplegray.desktop DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_DATA_DIR} )
endif(APPLE)


