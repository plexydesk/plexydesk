# Desk :
# Project Author: siraj@plexydesk.com
# CMAKE files by: siraj@plexydesk.com
#                 PhobosK (phobosk@kbfx.net)
#Copyright (c) 2015, Plexydesk
PROJECT(PlexyDesk)

CMAKE_MINIMUM_REQUIRED(VERSION 3.0.1)

CMAKE_POLICY(SET CMP0003 OLD)
CMAKE_POLICY(SET CMP0020 NEW) #Windows warning link qtmain
CMAKE_POLICY(SET CMP0043 NEW)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#Windows Pre Configure
include(configure_windows.cmake)

#handle platform options
option(BUILD_WITH_QT "Enable Qt ToolKit to Build PlexyDesk" ON)
option(BUILD_OPTIMIZED "Enable Compiler Optimizations" OFF)
option(ENABLE_DEVELOPER_BUILD "Build PlexyDesk without Code Signing" ON)

if(BUILD_WITH_QT)
  add_definitions(-DBUILD_WITH_QT=1)
  add_definitions(-D__QT_TOOLKIT__)
  add_definitions(-D__QT5_TOOLKIT__)
  set(ENABLE_QT5 TRUE)
endif()

if(BUILD_OPTIMIZED)
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native")
  endif()
endif()
#check if the compiler support c++11
include(CheckCXXCompilerFlag)

if (NOT WIN32)
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
	message(STATUS " The compiler ${CMAKE_CXX_COMPILER} supports c++11 - no")
endif()

if(COMPILER_SUPPORTS_CXX11_EXTENSIONS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11-extensions")
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} supports C++11-extensions")
else()
	message(STATUS "The compiler ${CMAKE_CXX_COMPILER}  C++11-extensions support - no")
endif()
endif()

#cmake options.
if (NOT SOCIAL_KIT_FLICKR_API_KEY)
   message(WARNING " Flickr Developer key not defined by SOCIAL_KIT_FLICKR_API_KEY, Image Search will not function")
endif()

if (NOT SOCIAL_KIT_DROPBOX_API_KEY)
   message(WARNING " Dropbox Developer key not defined by SOCIAL_KIT_DROPBOX_API_KEY, Remote Storage will not function")
endif()

if (NOT SOCIAL_KIT_PIXABAY_API_KEY)
   message(WARNING " Pixabay Developer key not defined by SOCIAL_KIT_PIXABAY_API_KEY, Pixbay Wallpaper search will not function")
endif()

if (NOT SOCIAL_KIT_CLIENT_ID)
   message(WARNING "SOCIAL_KIT_CLIENT_ID Not defined Social Authentication will not function")
endif()

if (NOT SOCIAL_KIT_CLIENT_SECRET)
   message(WARNING "SOCIAL_KIT_CLIENT_SECRET Not defined Social Authentication will not function")
endif()


set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmakemodules)

# Needed/optional packages definitions
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(Qt5Network REQUIRED)

#plugins
if (APPLE)
    set (CMAKE_PLEXYDESK_OSX_PLUGIN_PREFIX "plexydesk.app/Contents/PlugIns/plexydesk/")
    set (CMAKE_PLEXYDESK_OSX_PLUGIN_DATA_DIR "plexydesk.app/Contents/PlugIns/plexydesk/data")
endif()

set(QT_REQUIRED_VERSION 5.2.0)
find_package(Qt5 ${QT_REQUIRED_VERSION} REQUIRED Gui Core Xml Widgets Network)


if (UNIX AND NOT APPLE)
  find_package(Threads REQUIRED)
  find_package(X11 REQUIRED)

  if (${X11_FOUND})
    add_definitions(-D__X11_PLATFORM__)
    find_package(Qt5X11Extras)
  endif()
endif(UNIX AND NOT APPLE)

if (UNIX AND NOT APPLE)
    if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        add_definitions(-D__GNU_LINUX_PLATFORM__)
    endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

    if(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
        add_definitions(-D__FREE_BSD_PLATFORM__)
    endif(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
endif()

if (APPLE)
  add_definitions(-D__APPLE_PLATFORM__)
  
  set(CMAKE_MACOSX_RPATH NO)
  set(CMAKE_INSTALL_NAME_DIR "@executable_path/../frameworks")
endif()

# Project variables that need to be defined manually
set(LIB_MAJOR 1)
set(LIB_MINOR 0)
set(LIB_RELEASE 3)

set(APPLICATION_INTERNAL_VERSION 20150101)
set(APPLICATION_DATE 2015-01-01)

# Auto defined project variables
set(APPLICATION_NAME "${PROJECT_NAME}")
set(APPLICATION_MAIN_VERSION "${LIB_MAJOR}.${LIB_MINOR}.${LIB_RELEASE}")

#Library Names
if (BUILD_WITH_QT)
  set(APPLICATION_EXE_NAME plexydesk)
        set(PLEXY_UI_CORE_LIBRARY CherryKitQt)
  set(QUETZAL_SOCIAL_KIT_LIBRARY QuetzalSocialKitQt)
  set(QUETZAL_DATA_KIT_LIBRARY QuetzalDataKitQt)
else()
  set(APPLICATION_EXE_NAME plexydesk)
        set(PLEXY_UI_CORE_LIBRARY CherryKit)
  set(QUETZAL_SOCIAL_KIT_LIBRARY QuetzalSocialKit)
  set(QUETZAL_DATA_KIT_LIBRARY QuetzalDataKit)
endif()

if (APPLE)
    set(MACOSX_BUNDLE_EXECUTABLE_NAME "plexydesk")
    set(MACOSX_BUNDLE_INFO_STRING "PlexyDesk")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER ${MAC_BUNDLE_ID})
    set(MACOSX_BUNDLE_LONG_VERSION_STRING "plexydesk 1.0-mac")
    set(MACOSX_BUNDLE_BUNDLE_NAME "org.plexydesk.mac")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "1.0")
    set(MACOSX_BUNDLE_COPYRIGHT "© 2014, Siraj Razick")
    set(MACOSX_BUNDLE_CLASS "NSApplication")

    set(RESOURCES_DEST_DIR plexydesk.app/Contents/Resources/icons/)
endif (APPLE)

if (NOT APPLE AND UNIX)
    set(RESOURCES_DEST_DIR share/icons/plexydesk/resources/icons/)
endif ()

#set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS})
if (NOT WIN32)
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif (NOT WIN32)

if (APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -gdwarf-2")
endif (APPLE)


# Global variable CMAKE_BUILD_TYPE handling
# None (CMAKE_C_FLAGS or CMAKE_CXX_FLAGS used)
# Debug (CMAKE_C_FLAGS_DEBUG or CMAKE_CXX_FLAGS_DEBUG)
# Release (CMAKE_C_FLAGS_RELEASE or CMAKE_CXX_FLAGS_RELEASE)
# RelWithDebInfo (CMAKE_C_FLAGS_RELWITHDEBINFO or CMAKE_CXX_FLAGS_RELWITHDEBINFO
# MinSizeRel (CMAKE_C_FLAGS_MINSIZEREL or CMAKE_CXX_FLAGS_MINSIZEREL)
# By default we should set it to Release. This can be overridden by a -D command
# line switch.
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release
        CACHE
        STRING "Specify build type: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif (NOT CMAKE_BUILD_TYPE)

# Check if we use any Debug in the final release and if so use console in Windows
# For the targets add ${WIN_APPLICATION} as second argument if you want a non
# console application like:
# example: ADD_EXECUTABLE(exe_target ${WIN_APPLICATION} ${sourceFiles})
if (CMAKE_BUILD_TYPE MATCHES "Debug")
    MESSAGE(STATUS "Setting Debug flags ...")

    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer -O0 -g")
    endif()

    set(WIN_APPLICATION "")
else(CMAKE_BUILD_TYPE MATCHES "Release")
    # Note: the WIN32 is ignored on other platforms than Windows
    message(STATUS "-- Setting Release flags ...")
    set(WIN_APPLICATION "WIN32")
endif()

INCLUDE(PkgConfigGetVar)
INCLUDE(GNUInstallDirs)

#set(QT_IMPORTS_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/qt5/imports)

# On Windows we use a predefined QT_IMPORTS_INSTALL_DIR
if (NOT WIN32)
    # Workaround for platforms that do not provide it (Ubuntu)
    IF(NOT QT_IMPORTS_DIR)
	    MESSAGE(STATUS "QT_IMPORTS_DIR not present... use fallback: ${CMAKE_INSTALL_FULL_LIBDIR}/qt5/imports")
        set(QT_IMPORTS_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/qt5/imports)
    ELSE(NOT QT_IMPORTS_DIR)
        # Making QT_IMPORTS_INSTALL_DIR relative. This is a dirty hack but should work
        STRING(REGEX REPLACE "^/usr/" "" QT_IMPORTS_INSTALL_DIR "${QT_IMPORTS_DIR}")
        MESSAGE(STATUS "Relative QT Imports path set to: ${QT_IMPORTS_INSTALL_DIR}")
    endif(NOT QT_IMPORTS_DIR)
endif(NOT WIN32)

INCLUDE_DIRECTORIES (
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    )

if (MSVC AND MSVC_VERSION GREATER 1500)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MD")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MD")

    if (ENABLE_WINRT)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /ZW -DDEFINE_LITTLE_ENDIAN -DDEFINE_TRACE -DDEFINE_WINDOWS_UNIVERSAL -D_CRT_SECURE_NO_WARNINGS")
    endif()
endif(MSVC AND MSVC_VERSION GREATER 1500)

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
if (MINGW)
    ADD_CUSTOM_COMMAND(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/wally_rc.o
        COMMAND windres.exe
        -I ${CMAKE_CURRENT_SOURCE_DIR}/dist/windows
        -o ${CMAKE_CURRENT_BINARY_DIR}/dist/windows/${APPLICATION_EXE_NAME}_rc.o
        -i ${CMAKE_CURRENT_BINARY_DIR}/dist/windows/windres.rc
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/dist/windows
        COMMENT "Generating windows RC file ...")
endif(MINGW)

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
    ${CMAKE_SOURCE_DIR}/deps/json-cpp/
    ${CMAKE_SOURCE_DIR}/deps/json-cpp/json/
    ${QT_INCLUDE_DIR}
    )

INCLUDE(CPack)

add_subdirectory(deps/json-cpp)
add_subdirectory(deps/tinyxml2)

if (WIN32)
    ADD_SUBDIRECTORY(dist/windows)
endif(WIN32)


# *** ALL PLATFORMS ***
#ADD_SUBDIRECTORY(base/core)
add_subdirectory(base/uikit)

if (NOT APPLE)
  if (NOT ENABLE_WINRT)
    ADD_SUBDIRECTORY(runner)
  endif()
endif()

if (APPLE)
    ADD_SUBDIRECTORY(runner/apple)
endif()

if (ENABLE_WINRT)
   add_subdirectory(runner/windows_uwp/desktop/)
endif()

#todo build only on debug
add_subdirectory(base/social/data)
add_subdirectory(base/datastore/lib)
add_subdirectory(base/social/lib)
add_subdirectory(base/social/testrunner)

if (UNIX)
	add_subdirectory(extensions/runtime)
	add_subdirectory(base/datastore/tests)
	add_subdirectory(extensions/mock_desk)
endif ()

add_subdirectory(extensions/widgets/clock)
add_subdirectory(extensions/widgets/deskcontrol)
add_subdirectory(extensions/widgets/photoframe)
#ADD_SUBDIRECTORY(extensions/widgets/folderwidget)
add_subdirectory(extensions/background/default)
add_subdirectory(extensions/data/flickrphotosearch)
add_subdirectory(extensions/data/httpserver)
add_subdirectory(extensions/widgets/desktopnotes)
add_subdirectory(extensions/widgets/date)
add_subdirectory(extensions/widgets/desksync)
add_subdirectory(extensions/widgets/template)

if (${ENABLE_JEFF})
add_subdirectory(extensions/widgets/nlp)
add_subdirectory(extensions/widgets/nlp/tools/query_jeff)
add_subdirectory(extensions/widgets/nlp/tools/train_jeff)
endif()

add_subdirectory(extensions/widgets/dock)

add_subdirectory(extensions/data/rest)
add_subdirectory(extensions/data/timer)

add_subdirectory(extensions/activity/)

if (APPLE)
 #this style works only on Mac OS.
 add_subdirectory(extensions/style/cocoa)
endif()

add_subdirectory(extensions/style/simple_gray)

add_subdirectory(artwork/packs/)
