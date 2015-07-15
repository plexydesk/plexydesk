set(sourceFiles
    ubuntu.cpp
    ubuntuinterface.cpp
    )

set(headerFiles
    ubuntu.h
    ubuntuinterface.h
    )

set(QTMOC_SRCS
    ubuntu.h
    ubuntuinterface.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

set(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

set(libs
    ${QT_QTCORE_LIBRARY}
    )

ADD_LIBRARY(ubuntustyle SHARED ${sourceFiles} ${QT_MOC_SRCS})

if (MINGW)
    SET_TARGET_PROPERTIES(ubuntustyle PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
endif(MINGW)

TARGET_LINK_LIBRARIES(ubuntustyle
    ${PLEXY_CORE_LIBRARY}
    ${libs}
    )

QT5_USE_MODULES(ubuntustyle Widgets Xml  Core  )

if (NOT APPLE)
INSTALL(TARGETS ubuntustyle DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES ubuntu.desktop DESTINATION share/plexy/ext/groups)
endif (NOT APPLE)

if (APPLE)
   INSTALL(TARGETS ubuntustyle LIBRARY DESTINATION plexydesk.app/Contents/lib/plexyext/ COMPONENT Framework)
   INSTALL(FILES ubuntu.desktop DESTINATION plexydesk.app/Contents/share/plexy/ext/groups)
endif(APPLE)


