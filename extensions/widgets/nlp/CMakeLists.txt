set(sourceFiles
    nlp.cpp
    nlpinterface.cpp
    )

set(headerFiles
    nlp.h
    nlpinterface.h
    )

set(QTMOC_SRCS
    nlp.h
    nlpinterface.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

set(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

set(libs
    ${QT_QTCORE_LIBRARY}
    )

ADD_LIBRARY(nlpwidget SHARED ${sourceFiles} ${QT_MOC_SRCS})

if (MINGW)
    SET_TARGET_PROPERTIES(nlpwidget PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
endif(MINGW)

TARGET_LINK_LIBRARIES(nlpwidget
    ${PLEXY_UI_CORE_LIBRARY}
    ${libs}
    )

QT5_USE_MODULES(nlpwidget Widgets Xml  Core   )

if (NOT APPLE)
INSTALL(TARGETS nlpwidget DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES nlp.desktop DESTINATION share/plexy/ext/groups)
endif (NOT APPLE)

if (APPLE)
   INSTALL(TARGETS nlpwidget LIBRARY DESTINATION plexydesk.app/Contents/lib/plexyext/ COMPONENT Framework)
   INSTALL(FILES nlp.desktop DESTINATION plexydesk.app/Contents/share/plexy/ext/groups)
endif(APPLE)


