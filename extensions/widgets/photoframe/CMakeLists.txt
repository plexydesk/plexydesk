set(CMAKE_AUTOMOC ON)

ADD_SUBDIRECTORY(img)

SET(sourceFiles
    interface.cpp
    imageplugin.cpp
    photowidget.cpp
    )

SET(headerFiles
    interface.h
    imageplugin.h
    photowidget.h
    )

SET(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

ADD_LIBRARY(photoframe SHARED ${sourceFiles})

QT5_USE_MODULES(photoframe Widgets Xml OpenGL Core WebKitWidgets)

TARGET_LINK_LIBRARIES(photoframe
    ${QT_QTCORE_LIBRARY}
    ${PLEXY_UI_CORE_LIBRARY}
    )

IF (NOT APPLE)
INSTALL(TARGETS photoframe DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES photoframe.desktop DESTINATION share/plexy/ext/groups)
ENDIF (NOT APPLE)

IF (APPLE)
   INSTALL(TARGETS photoframe LIBRARY DESTINATION plexydesk.app/Contents/lib/plexyext/ COMPONENT Framework)
   INSTALL(FILES photoframe.desktop DESTINATION plexydesk.app/Contents/share/plexy/ext/groups)
ENDIF (APPLE)
