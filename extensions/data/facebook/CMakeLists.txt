set(sourceFiles
     facebooksession.cpp
     facebookdatainterface.cpp
     facebookuserinfo.cpp
    )

set(headerFiles facebooksession.h facebookdatainterface.h facebookuserinfo.h
    )

set(QTMOC_SRCS facebooksession.h facebookdatainterface.h facebookuserinfo.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

set(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

set(libs
    jsoncpp
    )

ADD_LIBRARY(facebookengine SHARED ${sourceFiles} ${QT_MOC_SRCS})

if (MINGW)
    SET_TARGET_PROPERTIES(facebookengine PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
endif(MINGW)

TARGET_LINK_LIBRARIES(facebookengine
    ${PLEXY_CORE_LIBRARY}
    ${libs}
    )

QT5_USE_MODULES(facebookengine Widgets Xml OpenGL Core   )

if (NOT APPLE)
INSTALL(TARGETS facebookengine DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES facebook.desktop DESTINATION share/plexy/ext/groups)
endif (NOT APPLE)

if (APPLE)
   INSTALL(TARGETS facebookengine LIBRARY DESTINATION plexydesk.app/Contents/lib/plexyext/ COMPONENT Framework)
   INSTALL(FILES facebook.desktop DESTINATION plexydesk.app/Contents/share/plexy/ext/groups)
endif(APPLE)
