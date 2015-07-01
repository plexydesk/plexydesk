# Desk :
# Project Author: siraj@plexydesk.org
# CMAKE files by: siraj@plexydesk.org
#                 PhobosK (phobosk@kbfx.net)
#Copyright (c) 2015, Plexydesk
PROJECT(PlexyDesk)

CMAKE_MINIMUM_REQUIRED(VERSION 3.0.1)

CMAKE_POLICY(SET CMP0003 OLD)
CMAKE_POLICY(SET CMP0020 NEW) #Windows warning link qtmain
CMAKE_POLICY(SET CMP0043 NEW)

#check if the compiler support c++11
include(CheckCXXCompilerFlag)

CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
CHECK_CXX_COMPILER_FLAG("-std=c++11-extensions" COMPILER_SUPPORTS_CXX11_EXTENSIONS)

if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} supports C++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} supports C++10x")
else()
    message(ERROR " The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

if(COMPILER_SUPPORTS_CXX11_EXTENSIONS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11-extensions")
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} supports C++11-extensions")
else()
    message(ERROR " The compiler ${CMAKE_CXX_COMPILER} has no C++11-extensions support. Please use a different C++ compiler.")
endif()

#cmake options.
if (NOT SOCIAL_KIT_FLICKR_API_KEY)
   message(ERROR " Flickr Developer key not defined by SOCIAL_KIT_FLICKR_API_KEY, Image Search will not function")
endif()

if (NOT SOCIAL_KIT_DROPBOX_API_KEY)
   message(ERROR " Dropbox Developer key not defined by SOCIAL_KIT_DROPBOX_API_KEY, Remote Storage will not function")
endif()

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmakemodules)

# Needed/optional packages definitions
FIND_PACKAGE(Qt5Core REQUIRED)
FIND_PACKAGE(Qt5Widgets REQUIRED)
FIND_PACKAGE(Qt5Xml REQUIRED)
FIND_PACKAGE(Qt5Gui REQUIRED)
FIND_PACKAGE(Qt5OpenGL REQUIRED)
FIND_PACKAGE(Qt5Network REQUIRED)

FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(MITIE REQUIRED)

#plugins
if (APPLE)
    set (CMAKE_PLEXYDESK_OSX_PLUGIN_PREFIX "plexydesk.app/Contents/PlugIns/plexydesk/")
    set (CMAKE_PLEXYDESK_OSX_PLUGIN_DATA_DIR "plexydesk.app/Contents/PlugIns/plexydesk/data")
endif()

set(QT_REQUIRED_VERSION 5.2.0)
find_package(Qt5 ${QT_REQUIRED_VERSION} REQUIRED Gui Core Xml Widgets Network)


IF(UNIX AND NOT APPLE)
    FIND_PACKAGE(Qt5X11Extras)
ENDIF(UNIX AND NOT APPLE)

# Project variables that need to be defined manually
SET(LIB_MAJOR 1)
SET(LIB_MINOR 0)
SET(LIB_RELEASE 0)

SET(APPLICATION_INTERNAL_VERSION 20150101)
SET(APPLICATION_DATE 2015-01-01)

# Auto defined project variables
SET(APPLICATION_NAME "${PROJECT_NAME}")
SET(APPLICATION_MAIN_VERSION "${LIB_MAJOR}.${LIB_MINOR}.${LIB_RELEASE}")

#Library Names
SET(APPLICATION_EXE_NAME plexydesk)
SET(PLEXY_UI_CORE_LIBRARY PlexyDeskUICore)
SET(QUETZAL_SOCIAL_KIT_LIBRARY QuetzalSocialKit)
SET(QUETZAL_DATA_KIT_LIBRARY QuetzalDataKit)

# Check on Windows if we have used CMAKE_INSTALL_PREFIX or default it to
# outsource special ready path
# Set the install prefix
IF(WIN32)
    IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        SET(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/${APPLICATION_EXE_NAME}-ready-build" CACHE PATH "default install path" FORCE)
    ENDIF()
    STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
    MESSAGE(STATUS "CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}")
ENDIF(WIN32)

IF (APPLE)
    SET(MACOSX_BUNDLE_EXECUTABLE_NAME "plexydesk")
    SET(MACOSX_BUNDLE_INFO_STRING "PlexyDesk")
    SET(MACOSX_BUNDLE_GUI_IDENTIFIER ${MAC_BUNDLE_ID})
    SET(MACOSX_BUNDLE_LONG_VERSION_STRING "plexydesk 1.0-mac")
    SET(MACOSX_BUNDLE_BUNDLE_NAME "org.plexydesk.mac")
    SET(MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0")
    SET(MACOSX_BUNDLE_BUNDLE_VERSION "1.0")
    SET(MACOSX_BUNDLE_COPYRIGHT "© 2014, Siraj Razick")
    SET(MACOSX_BUNDLE_CLASS "NSApplication")

    SET(RESOURCES_DEST_DIR plexydesk.app/Contents/share/plexy/themepack/default/resources)
ENDIF (APPLE)

IF (NOT APPLE)
    SET(RESOURCES_DEST_DIR share/plexy/themepack/default/resources)
ENDIF (NOT APPLE)

#set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS})
IF (NOT WIN32)
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
ENDIF (NOT WIN32)

IF (APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -gdwarf-2")
ENDIF (APPLE)

# Global variable CMAKE_BUILD_TYPE handling
# None (CMAKE_C_FLAGS or CMAKE_CXX_FLAGS used)
# Debug (CMAKE_C_FLAGS_DEBUG or CMAKE_CXX_FLAGS_DEBUG)
# Release (CMAKE_C_FLAGS_RELEASE or CMAKE_CXX_FLAGS_RELEASE)
# RelWithDebInfo (CMAKE_C_FLAGS_RELWITHDEBINFO or CMAKE_CXX_FLAGS_RELWITHDEBINFO
# MinSizeRel (CMAKE_C_FLAGS_MINSIZEREL or CMAKE_CXX_FLAGS_MINSIZEREL)
# By default we should set it to Release. This can be overridden by a -D command
# line switch.
IF (NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Release
        CACHE
        STRING "Specify build type: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
ENDIF (NOT CMAKE_BUILD_TYPE)

# Check if we use any Debug in the final release and if so use console in Windows
# For the targets add ${WIN_APPLICATION} as second argument if you want a non
# console application like:
# example: ADD_EXECUTABLE(exe_target ${WIN_APPLICATION} ${sourceFiles})
IF(CMAKE_BUILD_TYPE MATCHES "Debug")
    MESSAGE(STATUS "Setting Debug flags ...")
    SET(WIN_APPLICATION "")
ELSE(CMAKE_BUILD_TYPE MATCHES "Debug")
    # Note: the WIN32 is ignored on other platforms than Windows
    SET(WIN_APPLICATION "WIN32")
ENDIF(CMAKE_BUILD_TYPE MATCHES "Debug")

INCLUDE(PkgConfigGetVar)
INCLUDE(GNUInstallDirs)

SET(QT_IMPORTS_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/qt5/imports)

# On Windows we use a predefined QT_IMPORTS_INSTALL_DIR
IF (NOT WIN32)

    # Workaround for platforms that do not provide it (Ubuntu)
    IF(NOT QT_IMPORTS_DIR)
        MESSAGE("QT_IMPORTS_DIR not present...\nUsing a built in one: ${CMAKE_INSTALL_FULL_LIBDIR}/qt4/imports")
        SET(QT_IMPORTS_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/qt5/imports)
    ELSE(NOT QT_IMPORTS_DIR)
        # Making QT_IMPORTS_INSTALL_DIR relative. This is a dirty hack but should work
        STRING(REGEX REPLACE "^/usr/" "" QT_IMPORTS_INSTALL_DIR "${QT_IMPORTS_DIR}")
        MESSAGE(STATUS "Relative QT Imports path set to: ${QT_IMPORTS_INSTALL_DIR}")
    ENDIF(NOT QT_IMPORTS_DIR)

ENDIF(NOT WIN32)

INCLUDE_DIRECTORIES (
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    )

IF(MSVC AND MSVC_VERSION GREATER 1500)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
ENDIF(MSVC AND MSVC_VERSION GREATER 1500)

IF(WIN32)
    SET(FFMPEG_INCLUDE_DIR ${CMAKE_BINARY_DIR}/win32/ffmpeg/include)
    SET(FFMPEG_LIBRARIES avcodec avdevice avformat avutil)
    SET(FFMPEG_FOUND TRUE)
ENDIF(WIN32)

CONFIGURE_FILE(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h )

#doxygen
find_package (Doxygen)
CONFIGURE_FILE(plexydesk.Doxyfile.cmake ${CMAKE_CURRENT_BINARY_DIR}/plexydesk.Doxyfile )

if (DOXYGEN_FOUND)
    execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/docs/api/)
    add_custom_target (doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/plexydesk.Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs/api
        COMMENT "Generating API documentation with Doxygen" VERBATIM
        )
endif (DOXYGEN_FOUND)

CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/runner/plexydesk.cmake.desktop"
    "${CMAKE_CURRENT_BINARY_DIR}/runner/plexydesk.desktop"
    @ONLY)

CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/dist/windows/make_nsis_installer.cmake.nsi"
    "${CMAKE_CURRENT_BINARY_DIR}/dist/windows/make_nsis_installer.nsi"
    @ONLY)
CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmakemodules/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
    "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

# *** Windows Resource file handling ***
# Put all the resource files for Windows in this section using the pattern bellow
# For every rc file a "CONFIGURE_FILE" and a "IF(MINGW)" is needed
# Then add the resource to the sources files as "resourceFiles" and use them
# in the final target to be compiled (for an example see the runner/CMakeLists.txt)
CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/dist/windows/${APPLICATION_EXE_NAME}_windres.rc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/dist/windows/${APPLICATION_EXE_NAME}_windres.rc")
IF(MINGW)
    ADD_CUSTOM_COMMAND(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/wally_rc.o
        COMMAND windres.exe
        -I ${CMAKE_CURRENT_SOURCE_DIR}/dist/windows
        -o ${CMAKE_CURRENT_BINARY_DIR}/dist/windows/${APPLICATION_EXE_NAME}_rc.o
        -i ${CMAKE_CURRENT_BINARY_DIR}/dist/windows/windres.rc
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/dist/windows
        COMMENT "Generating windows RC file ...")
ENDIF(MINGW)

INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/base/uikit
    ${CMAKE_BINARY_DIR}/base/uikit
    ${CMAKE_SOURCE_DIR}/base/uikit/widgets/
    ${CMAKE_BINARY_DIR}/base/uikit/widets/
    ${CMAKE_BINARY_DIR}/base/uikit/components/
    ${CMAKE_SOURCE_DIR}/base/uikit/components/
    ${CMAKE_SOURCE_DIR}/base/uikit/core
    ${CMAKE_BINARY_DIR}/base/uikit/core
    ${CMAKE_SOURCE_DIR}/base/social/lib
    ${CMAKE_BINARY_DIR}/base/social/lib
    ${CMAKE_SOURCE_DIR}/base/datastore/lib
    ${CMAKE_BINARY_DIR}/base/datastore/lib
    ${CMAKE_SOURCE_DIR}/runner
    ${CMAKE_BINARY_DIR}/runner
    ${CMAKE_SOURCE_DIR}/dist/windows
    ${CMAKE_BINARY_DIR}/dist/windows
    ${CMAKE_SOURCE_DIR}/deps/mitie/mitielib/include
    ${CMAKE_SOURCE_DIR}/deps/mitie/dlib/
    ${QT_INCLUDE_DIR}
    )

ADD_SUBDIRECTORY(deps/tinyxml2)

IF(WIN32)
    ADD_SUBDIRECTORY(dist/windows)
ENDIF(WIN32)

# *** ALL PLATFORMS ***
#ADD_SUBDIRECTORY(base/core)
ADD_SUBDIRECTORY(base/uikit)

if (NOT APPLE)
    ADD_SUBDIRECTORY(runner)
endif()

if (APPLE)
    ADD_SUBDIRECTORY(runner/apple)
endif()

ADD_SUBDIRECTORY(extensions/runtime)

ADD_SUBDIRECTORY(base/social/data)
ADD_SUBDIRECTORY(base/datastore/lib)
ADD_SUBDIRECTORY(base/social/lib)
ADD_SUBDIRECTORY(base/social/testrunner)
ADD_SUBDIRECTORY(base/datastore/tests)

ADD_SUBDIRECTORY(extensions/widgets/clock)
ADD_SUBDIRECTORY(extensions/widgets/photoframe)
#ADD_SUBDIRECTORY(extensions/widgets/folderwidget)
ADD_SUBDIRECTORY(extensions/background/default)
ADD_SUBDIRECTORY(extensions/data/flickrphotosearch)
ADD_SUBDIRECTORY(extensions/data/httpserver)
ADD_SUBDIRECTORY(extensions/widgets/desktopnotes)
ADD_SUBDIRECTORY(extensions/widgets/date)
ADD_SUBDIRECTORY(extensions/widgets/template)
ADD_SUBDIRECTORY(extensions/widgets/nlp)
ADD_SUBDIRECTORY(extensions/widgets/nlp/tools/query_jeff)
ADD_SUBDIRECTORY(extensions/widgets/nlp/tools/train_jeff)

ADD_SUBDIRECTORY(extensions/data/rest)
ADD_SUBDIRECTORY(extensions/data/timer)
ADD_SUBDIRECTORY(extensions/widgets/dock)
ADD_SUBDIRECTORY(extensions/activity/inputdialogactivity)
ADD_SUBDIRECTORY(extensions/activity/chooser)
#ADD_SUBDIRECTORY(extensions/activity/photosearch)
#ADD_SUBDIRECTORY(extensions/activity/flikrsearch)
ADD_SUBDIRECTORY(extensions/activity/progressdialog)
ADD_SUBDIRECTORY(extensions/activity/socialauth)
ADD_SUBDIRECTORY(extensions/activity/takenote)
ADD_SUBDIRECTORY(extensions/activity/datepicker)
ADD_SUBDIRECTORY(extensions/activity/timepicker)
#ADD_SUBDIRECTORY(extensions/activity/desktopclock)
ADD_SUBDIRECTORY(extensions/activity/timezone)
ADD_SUBDIRECTORY(extensions/activity/spacepreview)

ADD_SUBDIRECTORY(extensions/activity/template)
ADD_SUBDIRECTORY(extensions/style/cocoa)
ADD_SUBDIRECTORY(artwork)

