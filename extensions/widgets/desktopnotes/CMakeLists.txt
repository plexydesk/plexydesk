set(CMAKE_AUTOMOC ON)

set(sourceFiles
    desktopnotes.cpp
    desktopnotesinterface.cpp
    notewidget.cpp
    )

set(headerFiles
    desktopnotes.h
    desktopnotesinterface.h
    notewidget.h
    )

set(QTMOC_SRCS
    desktopnotes.h
    desktopnotesinterface.h
    notewidget.h
    )

if (ENABLE_QT5)
  QT5_WRAP_CPP(QT_MOC_SRCS)
endif()

set(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

set(libs
    ${QT_QTCORE_LIBRARY}
    )

ADD_LIBRARY(desktopnoteswidget SHARED ${sourceFiles} ${QT_MOC_SRCS})

if (MINGW)
    SET_TARGET_PROPERTIES(desktopnoteswidget PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
endif(MINGW)

TARGET_LINK_LIBRARIES(desktopnoteswidget
    ${QUETZAL_SOCIAL_KIT_LIBRARY}
    ${PLEXY_UI_CORE_LIBRARY}
    ${libs}
    )

if (ENABLE_QT5)
  QT5_USE_MODULES(desktopnoteswidget Widgets Xml  Core  )
endif()

if (NOT APPLE)
    INSTALL(TARGETS desktopnoteswidget DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
    INSTALL(FILES desktopnotes.desktop DESTINATION share/plexy/ext/groups)
endif (NOT APPLE)

if (APPLE)
   INSTALL(TARGETS desktopnoteswidget LIBRARY DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_PREFIX} COMPONENT Framework)
   INSTALL(FILES desktopnotes.desktop DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_DATA_DIR} )
endif(APPLE)


