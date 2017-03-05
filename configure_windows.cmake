message(STATUS "Running configure_windows.cmake")

if (WIN32)

	#msvc parameters
	if(MSVC_VERSION GREATER 1700)
  		set(COMPILER_VERSION "12")
	elseif(MSVC_VERSION GREATER 1600)
  		set(COMPILER_VERSION "11")
	endif()

        #configure cpack paramets
	set(CPACK_INSTALL_PREFIX "c:/cpack_root")
	FILE (TO_NATIVE_PATH ${CMAKE_INSTALL_PREFIX} CMAKE_INSTALL_NATIVE_PREFIX)

	#winrt macro
	if (ENABLE_WINRT)
    		add_definitions(-D__WINDOWS_UWP_PLATFORM__)
		message(STATUS "Detected windows platform -- [WINRT]")
	else()
  		add_definitions(-D__WINDOWS_PLATFORM__)
		message(STATUS "Detected windows platform -- [WIN32]")
	endif()

	#tools

  	GET_FILENAME_COMPONENT(WINSDK_DIR
        	"[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows;CurrentInstallFolder]" REALPATH CACHE)
  	find_program(SIGNTOOL signtool
    		PATHS
    		${WINSDK_DIR}/bin)
  
  	if (SIGNTOOL)
     		message(STATUS "signtool.exe found .... [success]")
  	else()
     		message(FATAL_ERROR "signtool.exe is not found in your computer")
  	endif()

	#install

	# Check on Windows if we have used CMAKE_INSTALL_PREFIX or default it to
	# outsource special ready path
	# Set the install prefix
    	IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        	set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/${APPLICATION_EXE_NAME}-ready-build"
			CACHE PATH "default install path" FORCE)
    	endif()

    	STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

   	MESSAGE(STATUS "Program Install location ........ [${CMAKE_INSTALL_PREFIX}]")

    	set(RESOURCES_DEST_DIR resources/icons/)

endif(WIN32)

#old stuff
if (WIN32)
    set(FFMPEG_INCLUDE_DIR ${CMAKE_BINARY_DIR}/win32/ffmpeg/include)
    set(FFMPEG_LIBRARIES avcodec avdevice avformat avutil)
    set(FFMPEG_FOUND TRUE)
endif(WIN32)

if (WIN32)
set(CPACK_GENERATOR WIX)
set(CPACK_WIX_TEMPLATE "${CMAKE_SOURCE_DIR}/cmakemodules/plexydesk-windows-installer.in.txt")
set(CPACK_PACKAGE_NAME "plexydesk")
set(CPACK_PACKAGE_VENDOR "plexydesk.org")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Super Charged Desktop Widgets")
set(CPACK_PACKAGE_VERSION "${LIB_MAJOR}.${LIB_MINOR}.${LIB_RELEASE}")
set(CPACK_PACKAGE_VERSION_MAJOR "${LIB_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${LIB_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${LIB_RELEASE}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "PlexyDesk")
set(CPACK_CREATE_DESKTOP_LINKS plexydesk)
set(CPACK_PACKAGE_EXECUTABLES plexydesk "PlexyDesk (Cherry)")
set(CPACK_WIX_UPGRADE_GUID ${PACKAGE_GUID})
set(CPACK_WIX_PRODUCT_ICON "${CMAKE_SOURCE_DIR}/artwork/icons/plexydesk.ico")
set(CPACK_WIX_UI_DIALOG "${CMAKE_CURRENT_SOURCE_DIR}/artwork/icons/wix_install_side_banner.bmp")
set(CPACK_WIX_UI_BANNER "${CMAKE_CURRENT_SOURCE_DIR}/artwork/icons/wix_install_top_banner.bmp")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/License.txt")
set(CPACK_WIX_PROGRAM_MENU_FOLDER "PlexyDesk")
#set(CPACK_WIX_UI_REF "WixUI_Advanced")
endif()



