set(sourceFiles
    fbpostdialog.cpp
    fbpostdialoginterface.cpp
    )

set(headerFiles
    fbpostdialog.h
    fbpostdialoginterface.h
    )

set(QTMOC_SRCS
    fbpostdialog.h
    fbpostdialoginterface.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

set(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

set(libs
    ${QT_QTCORE_LIBRARY}
    )

ADD_LIBRARY(fbpostdialogactivity SHARED ${sourceFiles} ${QT_MOC_SRCS})

if (MINGW)
    SET_TARGET_PROPERTIES(fbpostdialogactivity PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
endif(MINGW)

TARGET_LINK_LIBRARIES(fbpostdialogactivity
    ${PLEXY_CORE_LIBRARY}
    ${PLEXY_UI_CORE_LIBRARY}
    ${libs}
    )

QT5_USE_MODULES(fbpostdialogactivity Widgets Xml OpenGL Core  )

if (NOT APPLE)
INSTALL(TARGETS fbpostdialogactivity DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES fbpostdialog.desktop DESTINATION share/plexy/ext/groups)
endif (NOT APPLE)

if (APPLE)
   INSTALL(TARGETS fbpostdialogactivity LIBRARY DESTINATION plexydesk.app/Contents/lib/plexyext/ COMPONENT Framework)
   INSTALL(FILES fbpostdialog.desktop DESTINATION plexydesk.app/Contents/share/plexy/ext/groups)
endif(APPLE)


