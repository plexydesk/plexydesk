PROJECT(libQuetzalDataKit)

CMAKE_MINIMUM_REQUIRED(VERSION 3.0.1)

include (GenerateExportHeader)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
CMAKE_POLICY(SET CMP0043 NEW)

#simple and easy library to sync xml.
if (NOT LIB_MAJOR)
    set(LIB_MAJOR 0)
endif (NOT LIB_MAJOR)

if (NOT LIB_MINOR)
    set(LIB_MINOR 7)
endif (NOT LIB_MINOR)

if (NOT LIB_RELEASE)
    set(LIB_RELEASE 1)
endif (NOT LIB_RELEASE)

if (NOT QUETZAL_DATA_KIT_LIBRARY )
    set(QUETZAL_DATA_KIT_LIBRARY  QuetzalDataKit)
endif (NOT QUETZAL_DATA_KIT_LIBRARY )

if (NOT CMAKE_INSTALL_LIBDIR)
    INCLUDE(GNUInstallDirs)
endif (NOT CMAKE_INSTALL_LIBDIR)

INCLUDE_DIRECTORIES (
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
)

# Find Qt5
if (NOT Qt5Core_FOUND)
    FIND_PACKAGE(Qt5Core REQUIRED)
    FIND_PACKAGE(Qt5Widgets REQUIRED)
    FIND_PACKAGE(Qt5Xml REQUIRED)

    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

    # TODO/FIXME: -DPLEXYCORE_STANDALONE is stuff to be resolved!
    MESSAGE(WARNING "\n\nBuilding lib as a minimal standalone library. Non-functional for plexydesk now!\n\n")
    set(PLEXYCORE_STANDALONE 1)
    ADD_DEFINITIONS(-DPLEXYCORE_STANDALONE)
endif (NOT Qt5Core_FOUND)

# do not use config.h now
ADD_DEFINITIONS(-DPLEXYPREFIX="${CMAKE_INSTALL_PREFIX}"
                -DPLEXYLIBDIR="${CMAKE_INSTALL_LIBDIR}"
                -DPLEXYQTIMPORTSDIR="${QT_IMPORTS_INSTALL_DIR}"
                -DPLEXYRESOURCESDIR="${RESOURCES_DEST_DIR}"
               )

set(sourceFiles
    ck_image_io.cpp
    ck_data_sync.cpp
    ck_sync_object.cpp
    ck_sync_engine_interface.cpp
    ck_memory_sync_engine.cpp
    ck_disk_engine.cpp
    px_bench_impl.cpp
   )

set(headerFiles
    ck_image_io.h
    ck_data_sync.h
    ck_sync_object.h
    ck_memory_sync_engine.h
    ck_sync_engine_interface.h
    ck_disk_engine.h
    px_bench.h
   )

if (${BUILD_WITH_QT})
  set (sourceFiles ${sourceFiles} qt5/qt_platform_image.cpp)
  set (headerFiles ${sourceFiles} qt5/qt_platform_image.h)
endif()

if (APPLE)
   INCLUDE_DIRECTORIES ( /System/Library/Frameworks )
   FIND_LIBRARY(COCOA_LIBRARY Cocoa)
   FIND_LIBRARY(GLUT_LIBRARY GLUT)
   FIND_LIBRARY(OpenGL_LIBRARY OpenGL)
   MARK_AS_ADVANCED (COCOA_LIBRARY
                     GLUT_LIBRARY
                     OpenGL_LIBRARY)
   set(MAC_LIBS ${COCOA_LIBRARY} ${GLUT_LIBRARY} ${OpenGL_LIBRARY})
endif(APPLE)

set(sourceFiles ${sourceFiles} ${headerFiles})

ADD_LIBRARY(${QUETZAL_DATA_KIT_LIBRARY} SHARED ${sourceFiles})


if (APPLE)
    SET_TARGET_PROPERTIES(${QUETZAL_DATA_KIT_LIBRARY} PROPERTIES FRAMEWORK 1)
endif(APPLE)


GENERATE_EXPORT_HEADER( ${QUETZAL_DATA_KIT_LIBRARY}
             BASE_NAME ${QUETZAL_DATA_KIT_LIBRARY}
             EXPORT_MACRO_NAME ${QUETZAL_DATA_KIT_LIBRARY}_EXPORT
             EXPORT_FILE_NAME ${QUETZAL_DATA_KIT_LIBRARY}_export.h
             STATIC_DEFINE ${QUETZAL_DATA_KIT_LIBRARY}_BUILT_AS_STATIC
)

if (MINGW OR WIN32)
    SET_TARGET_PROPERTIES(${QUETZAL_DATA_KIT_LIBRARY} PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
   )
endif(MINGW OR WIN32)


if  (UNIX AND NOT APPLE)
	set(THREAD_LIBS pthread)
endif()

TARGET_LINK_LIBRARIES(${QUETZAL_DATA_KIT_LIBRARY} ${MAC_LIBS} ${libs} 
	 ${THREAD_LIBS})

QT5_USE_MODULES(${QUETZAL_DATA_KIT_LIBRARY} Core Widgets Xml)


if (UNIX AND NOT APPLE)
    INSTALL(TARGETS ${QUETZAL_DATA_KIT_LIBRARY} DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif(UNIX AND NOT APPLE)

if (APPLE)
   SET_TARGET_PROPERTIES(${QUETZAL_DATA_KIT_LIBRARY} PROPERTIES SKIP_BUILD_RPATH 0)
   INSTALL(TARGETS ${QUETZAL_DATA_KIT_LIBRARY}
                    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
                    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
                    FRAMEWORK DESTINATION plexydesk.app/Contents/Frameworks)
endif(APPLE)

if(WIN32)
    # installs to the same path as the binary
    install(TARGETS ${QUETZAL_DATA_KIT_LIBRARY}
                                               DESTINATION .
                                               ARCHIVE DESTINATION sdk)
endif()
