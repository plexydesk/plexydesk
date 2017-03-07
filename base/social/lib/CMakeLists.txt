PROJECT(libsocial)

CMAKE_MINIMUM_REQUIRED(VERSION 3.0.1)
include (GenerateExportHeader)
set(CMAKE_AUTOMOC ON)
CMAKE_POLICY(SET CMP0043 NEW)

# These variables are usually set in the toplevel directory
# but for standalone lib building it should be reset on demand
if (NOT LIB_MAJOR)
    set(LIB_MAJOR 0)
endif (NOT LIB_MAJOR)

if (NOT LIB_MINOR)
    set(LIB_MINOR 7)
endif (NOT LIB_MINOR)

if (NOT LIB_RELEASE)
    set(LIB_RELEASE 1)
endif (NOT LIB_RELEASE)

if (NOT QUETZAL_SOCIAL_KIT_LIBRARY)
    set(QUETZAL_SOCIAL_KIT_LIBRARY coresocial)
endif (NOT QUETZAL_SOCIAL_KIT_LIBRARY)

if (NOT CMAKE_INSTALL_LIBDIR)
    INCLUDE(GNUInstallDirs)
endif (NOT CMAKE_INSTALL_LIBDIR)

if (UNIX AND NOT APPLE)
    find_package(LibSoup REQUIRED)
    find_package(GLIB REQUIRED)
endif()

# Find Qt5
if (ENABLE_QT5)
if (NOT Qt5Core_FOUND)
    FIND_PACKAGE(Qt5Core REQUIRED)
    FIND_PACKAGE(Qt5Widgets REQUIRED)
    FIND_PACKAGE(Qt5Xml REQUIRED)
    FIND_PACKAGE(Qt5Network REQUIRED)

    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

    # TODO/FIXME: -DPLEXYCORE_STANDALONE is stuff to be resolved!
    MESSAGE(WARNING "\n\nBuilding lib as a minimal standalone library. Non-functional for plexydesk now!\n\n")
    set(PLEXYCORE_STANDALONE 1)
    ADD_DEFINITIONS(-DPLEXYCORE_STANDALONE)
endif (NOT Qt5Core_FOUND)
endif()


 if (ENABLE_WINRT)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /ZW")
  endif()

# do not use config.h now
ADD_DEFINITIONS(-DPLEXYPREFIX="${CMAKE_INSTALL_PREFIX}"
                -DPLEXYLIBDIR="${CMAKE_INSTALL_LIBDIR}"
                -DPLEXYQTIMPORTSDIR="${QT_IMPORTS_INSTALL_DIR}"
                -DPLEXYRESOURCESDIR="${RESOURCES_DEST_DIR}"
               )

INCLUDE_DIRECTORIES (
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${LIBSOUP_INCLUDE_DIRS}
        ${GLIB_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/deps/tinyxml2/
)

if (UNIX AND NOT APPLE)
  set (PLATFORM_SOURCE_FILES ck_url_request_linux.cpp)
  set (PLATFORM_HEADERS_FILES ck_url_request_linux.h)
endif()

if (APPLE)
  set (PLATFORM_SOURCE_FILES ck_url_request_mac.mm)
  set (PLATFORM_HEADERS_FILES ck_url_request_mac.h)
endif()

if(WIN32)
  if (NOT ENABLE_WINRT)
  	set (PLATFORM_SOURCE_FILES ck_url_request_windows.cpp)
  	set (PLATFORM_HEADERS_FILES ck_url_request_windows.h)
  endif()

  if (ENABLE_WINRT)
     set (PLATFORM_HEADERS_FILES ck_url_request_windows.h)
     set (PLATFORM_SOURCE_FILES ck_url_request_windows_uwp.cpp)
  endif()
endif()

set(sourceFiles
    ck_url.cpp
    ck_remote_service.cpp
    ck_remote_account.cpp
    ck_remote_account_sync.cpp
    webservice.cpp
    asyncimageloader.cpp
    asyncimagecreator.cpp
    asyncdatadownloader.cpp
    webcontentinterface.cpp
    ${PLATFORM_SOURCE_FILES}
   )

set(headerFiles
    ck_url.h
    ck_remote_service.h
    ck_remote_account.h
    ck_remote_account_sync.h
    webservice.h
    asyncimageloader.h
    asyncimagecreator.h
    asyncdatadownloader.h
    webcontentinterface.h
    ${PLATFORM_HEADERS_FILES}
   )

if (APPLE)
   INCLUDE_DIRECTORIES ( /System/Library/Frameworks )
   FIND_LIBRARY(COCOA_LIBRARY Cocoa)
   FIND_LIBRARY(GLUT_LIBRARY GLUT)
   FIND_LIBRARY(OpenGL_LIBRARY OpenGL)
	 FIND_LIBRARY(CoreServices APPLE_CORE_SERVICE_FRAMEWORK_LIBRARY)
   MARK_AS_ADVANCED (COCOA_LIBRARY
                     GLUT_LIBRARY
                     OpenGL_LIBRARY
                     APPLE_CORE_SERVICE_FRAMEWORK_LIBRARY)

   set(APPLE_OSX_PLATFORM_LINK_LIBRARIES ${COCOA_LIBRARY} ${GLUT_LIBRARY} ${OpenGL_LIBRARY} 
		   ${APPLE_CORE_SERVICE_FRAMEWORK_LIBRARY})
endif(APPLE)

set(sourceFiles ${sourceFiles} ${headerFiles})

ADD_LIBRARY(${QUETZAL_SOCIAL_KIT_LIBRARY} SHARED ${sourceFiles})

if (APPLE)
    set (CMAKE_MACOSX_RPATH 1)
    SET_TARGET_PROPERTIES(${QUETZAL_SOCIAL_KIT_LIBRARY} PROPERTIES FRAMEWORK 1)
    SET_TARGET_PROPERTIES(${QUETZAL_SOCIAL_KIT_LIBRARY} PROPERTIES SKIP_BUILD_RPATH 0)
endif(APPLE)

GENERATE_EXPORT_HEADER( ${QUETZAL_SOCIAL_KIT_LIBRARY}
             BASE_NAME ${QUETZAL_SOCIAL_KIT_LIBRARY}
             EXPORT_MACRO_NAME ${QUETZAL_SOCIAL_KIT_LIBRARY}_EXPORT
             EXPORT_FILE_NAME ${QUETZAL_SOCIAL_KIT_LIBRARY}_export.h
             STATIC_DEFINE ${QUETZAL_SOCIAL_KIT_LIBRARY}_BUILT_AS_STATIC
)

GENERATE_EXPORT_HEADER( ${QUETZAL_SOCIAL_KIT_LIBRARY}
             BASE_NAME ${QUETZAL_SOCIAL_KIT_LIBRARY}
             EXPORT_MACRO_NAME DECL_SOCIAL_KIT_EXPORT
             EXPORT_FILE_NAME social_kit_export.h
             STATIC_DEFINE ${QUETZAL_SOCIAL_KIT_LIBRARY}_BUILT_AS_STATIC
)


if (MINGW)
    SET_TARGET_PROPERTIES(${QUETZAL_SOCIAL_KIT_LIBRARY} PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
   )
endif(MINGW)

if (UNIX AND NOT LINUX)
    set(PLATFORM_LINK_LIBRARIES ${LIBSOUP_LIBRARIES})
endif()
if (WIN32)
    set(PLATFORM_LINK_LIBRARIES winhttp)
endif()

if (ENABLE_WINRT)
  set_property(TARGET ${QUETZAL_SOCIAL_KIT_LIBRARY} PROPERTY VS_WINRT_COMPONENT TRUE)
endif()

TARGET_LINK_LIBRARIES(${QUETZAL_SOCIAL_KIT_LIBRARY}
    ${APPLE_OSX_PLATFORM_LINK_LIBRARIES}
    ${PLATFORM_LINK_LIBRARIES}
    jsoncpp_ck
    tinyxml2
    )

if (ENABLE_QT5)
  QT5_USE_MODULES(${QUETZAL_SOCIAL_KIT_LIBRARY} Core Widgets Xml Network)
endif()

if (ENABLE_QT4)
  QT4_USE_MODULES(${QUETZAL_SOCIAL_KIT_LIBRARY} Core Xml Gui Network)
endif()


if (UNIX AND NOT APPLE)
    INSTALL(TARGETS ${QUETZAL_SOCIAL_KIT_LIBRARY} DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif(UNIX AND NOT APPLE)

if (APPLE)
    INSTALL(TARGETS  ${QUETZAL_SOCIAL_KIT_LIBRARY}
                    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/wow/
                    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
                    FRAMEWORK DESTINATION plexydesk.app/Contents/Frameworks)
endif(APPLE)

if(WIN32)
    # installs to the same path as the binary
    install(TARGETS ${QUETZAL_SOCIAL_KIT_LIBRARY}
                                   RUNTIME DESTINATION .
                                   LIBRARY DESTINATION .
                                   ARCHIVE DESTINATION sdk)
   #install(TARGETS ${QUETZAL_SOCIAL_KIT_LIBRARY} DESTINATION ${CMAKE_INSTALL_PREFIX})
endif()
