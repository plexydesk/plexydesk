set(sourceFiles
    cocoa.cpp
    cocoainterface.cpp
    ck_platform_context.cpp
    ck_rectf.cpp
    )

set(headerFiles
    cocoa.h
    cocoainterface.h
    ck_platform_context.h
    ck_rectf.h
    )

set(QTMOC_SRCS
    cocoainterface.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

set(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

set(libs
    ${QT_QTCORE_LIBRARY}
    )


#Locate cococa framework on mac
if(APPLE)
    include_directories(/System/Library/Frameworks)

    find_library(CORE_GRAPHICS_LIBRARY ApplicationServices)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(GLUT_LIBRARY GLUT)
    find_library(OpenGL_LIBRARY OpenGL)


    mark_as_advanced(COCOA_LIBRARY
        CORE_GRAPHICS_LIBRARY
        GLUT_LIBRARY
        OpenGL_LIBRARY)

    set(LIBRARY_NATIVE_LINK_TARGETS
        ${CORE_GRAPHICS_LIBRARY}
        ${COCOA_LIBRARY}
        ${GLUT_LIBRARY}
        ${OpenGL_LIBRARY})
endif(APPLE)

ADD_LIBRARY(cocoastyle SHARED ${sourceFiles} ${QT_MOC_SRCS}
                              ${LIBRARY_NATIVE_LINK_TARGETS})

if (MINGW)
    SET_TARGET_PROPERTIES(cocoastyle PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
endif(MINGW)

TARGET_LINK_LIBRARIES(cocoastyle
    ${PLEXY_UI_CORE_LIBRARY}
    ${libs}
    )

QT5_USE_MODULES(cocoastyle Widgets Xml  Core  )

if (NOT APPLE)
INSTALL(TARGETS cocoastyle DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES cocoa.desktop DESTINATION share/plexy/ext/groups)
endif (NOT APPLE)

if (APPLE)
   INSTALL(TARGETS cocoastyle LIBRARY DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_PREFIX} COMPONENT Framework)
   INSTALL(FILES cocoa.desktop DESTINATION ${CMAKE_PLEXYDESK_OSX_PLUGIN_DATA_DIR} )
endif(APPLE)


