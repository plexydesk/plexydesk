SET(sourceFiles
    authplugin.cpp
    authwidgetinterface.cpp
    facebookcontactui.cpp
    contactlist.cpp
    contactlistitem.cpp
    facebookcontactcard.cpp
    facebookauthenticationwidget.cpp
    facebookmessagedialog.cpp
    facebookcontactcardbutton.cpp
    )

SET(headerFiles
    authplugin.h
    authwidgetinterface.h
    facebookcontactui.h
    contactlist.h
    contactlistitem.h
    facebookcontactcard.h
    facebookauthenticationwidget.h
    facebookmessagedialog.h
    facebookcontactcardbutton.h
)

SET(QTMOC_SRCS
    authplugin.h
    authwidgetinterface.h
    facebookcontactui.h
    contactlist.h
    contactlistitem.h
    facebookcontactcard.h
    facebookauthenticationwidget.h
    facebookmessagedialog.h
    facebookcontactcardbutton.h
)

#QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

#if (QIMAGE_BLITZ_FOUND)
#   INCLUDE_DIRECTORIES(${QIMAGE_BLITZ_INCLUDE_DIR}/qimageblitz/)
#   message (${QIMAGE_BLITZ_INCLUDE_DIR})
#   set(optional_libs ${QIMAGE_BLITZ_LIBRARY})
#ENDIF (QIMAGE_BLITZ_FOUND)

SET(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

#SET(libs
#    ${optional_libs}
#    )

#ADD_LIBRARY(plexyauth SHARED ${sourceFiles} ${QT_MOC_SRCS})

#QT5_USE_MODULES(plexyauth Widgets Xml OpenGL Core   Declarative)

#TARGET_LINK_LIBRARIES(plexyauth
#    ${PLEXY_UI_CORE_LIBRARY}
#    ${libs}
#    )

#IF (NOT APPLE)
#INSTALL(TARGETS plexyauth DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
#INSTALL(FILES authwidget.desktop DESTINATION share/plexy/ext/groups)
#ENDIF (NOT APPLE)

#IF (APPLE)
#   INSTALL(TARGETS plexyauth LIBRARY DESTINATION plexydesk.app/Contents/lib/plexyext/ COMPONENT Framework)
#   INSTALL(FILES authwidget.desktop DESTINATION plexydesk.app/Contents/share/plexy/ext/groups)
#ENDIF(APPLE)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
find_package(Qt5Core)
add_library(plexyauth SHARED ${sourceFiles})
qt5_use_modules(plexyauth Widgets Xml OpenGL Core   Declarative)
target_link_libraries(plexyauth ${PLEXY_UI_CORE_LIBRARY})

ADD_SUBDIRECTORY(data)
