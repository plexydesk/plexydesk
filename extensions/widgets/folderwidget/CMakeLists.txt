set(CMAKE_AUTOMOC ON)

set(sourceFiles
    interface.cpp
    folderplugin.cpp
    iconwidgetview.cpp
    folderitem.cpp
    folderprovider.cpp
    fileinforview.cpp
    plexydesktopiconprovider.cpp
    )

set(headerFiles
    interface.h
    folderplugin.h
    iconwidgetview.h
    folderitem.h
    folderprovider.h
    fileinforview.h
    plexydesktopiconprovider.h
    )

set(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

set(CONFIG_FILE folderwidget.desktop)

ADD_LIBRARY(folderwidget SHARED ${sourceFiles})

QT5_USE_MODULES(folderwidget Widgets Xml  Core  )

TARGET_LINK_LIBRARIES(folderwidget
    ${PLEXY_UI_CORE_LIBRARY}
    )

if (NOT APPLE)
  INSTALL(TARGETS folderwidget DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
  INSTALL(FILES folderwidget.desktop DESTINATION share/plexy/ext/groups)
endif (NOT APPLE)

if (APPLE)
   INSTALL(TARGETS folderwidget LIBRARY DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_PREFIX} COMPONENT Framework)
   INSTALL(FILES folderwidget.desktop DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_DATA_DIR} )
endif (APPLE)
