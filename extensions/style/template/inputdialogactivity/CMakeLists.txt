set(sourceFiles
    inputdialogactivity.cpp
    inputdialogactivityinterface.cpp
    )

set(headerFiles
    inputdialogactivity.h
    inputdialogactivityinterface.h
    )

set(QTMOC_SRCS
    inputdialogactivity.h
    inputdialogactivityinterface.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

set(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

set(libs
    ${QT_QTCORE_LIBRARY}
    )

ADD_LIBRARY(inputdialogactivityengine SHARED ${sourceFiles} ${QT_MOC_SRCS})

if (MINGW)
    SET_TARGET_PROPERTIES(inputdialogactivityengine PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
endif(MINGW)

TARGET_LINK_LIBRARIES(inputdialogactivityengine
    ${PLEXY_CORE_LIBRARY}
    ${libs}
    )

QT5_USE_MODULES(inputdialogactivityengine Widgets Xml  Core   )

if (NOT APPLE)
INSTALL(TARGETS inputdialogactivityengine DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES inputdialogactivity.desktop DESTINATION share/plexy/ext/groups)
endif (NOT APPLE)

if (APPLE)
   INSTALL(TARGETS inputdialogactivityengine LIBRARY DESTINATION plexydesk.app/Contents/lib/plexyext/ COMPONENT Framework)
   INSTALL(FILES inputdialogactivity.desktop DESTINATION plexydesk.app/Contents/share/plexy/ext/groups)
endif(APPLE)


