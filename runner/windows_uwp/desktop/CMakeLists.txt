set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP true)
#include(InstallRequiredSystemLibraries)


if(WIN32)
  INCLUDE_DIRECTORIES(
        ${CMAKE_SOURCE_DIR}/dist/windows
        ${CMAKE_BINARY_DIR}/dist/windows)

  set(resourceFiles ${CMAKE_BINARY_DIR}/dist/windows/${APPLICATION_EXE_NAME}_windres.rc)
endif(WIN32)

########### Source files ###############
set(mainFiles main.cpp)
set(headerFiles desktopmanager.h ${extra_headers})

set(QTMOC_SRCS ${extra_headers})

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

set(sourceFiles
	desktopmanager.cpp
	${sourceFiles}
	${headerFiles}
	${mainFiles}
	)

if(WIN32)
  set(GUI_TYPE WIN32)

  if (ENABLE_WINRT)
      set(winrt_libs windowsapp.lib)
  endif()

  ADD_DEFINITIONS(-DNODEFAULTLIB:library)

  set(libs
    ${QT_QTGUI_LIBRARY}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTMAIN_LIBRARY}
    Qt5::WinMain
    ${winrt_libs}
    )
endif(WIN32)

if(UNIX)
	set(libs
		${QT_QTGUI_LIBRARY}
		${QT_QTCORE_LIBRARY}
		${QT_QTOPENGL_LIBRARY}
		${QT_QTSQL_LIBRARY}
		${X11_LIBRARIES}
		${Qt5X11Extras_LIBRARIES}
		${extra_libs})
endif(UNIX)

########### Prepare files ###############
message("-- Building for windows UWP ${APPLICATION_EXE_NAME}")

ADD_EXECUTABLE(${APPLICATION_EXE_NAME} ${APP_TYPE} ${WIN_APPLICATION} ${sourceFiles} ${resourceFiles} ${QT_MOC_SRCS})

set_property(TARGET ${APPLICATION_EXE_NAME} PROPERTY VS_WINRT_COMPONENT TRUE)

#if(MSVC)
#	MSVC_SET_PROPER_CONSOLE_HANDLING(${APPLICATION_EXE_NAME})
#endif(MSVC)
#SET_PROPERTY(TARGET ${APPLICATION_EXE_NAME} PROPERTY QT5_NO_LINK_QTMAIN ON)
#SET_TARGET_PROPERTIES(${APPLICATION_EXE_NAME} PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
#SET_TARGET_PROPERTIES(${APPLICATION_EXE_NAME} PROPERTIES MINSIZEREL "/SUBSYSTEM:WINDOWS")


 if (ENABLE_WINRT)
                SET_TARGET_PROPERTIES(${APPLICATION_EXE_NAME} PROPERTIES LINK_FLAGS_RELEASE "/WINMD /OPT:REF /map Ws2_32.lib WindowsApp.lib /APPCONTAINER  /DYNAMICBASE /SUBSYSTEM:WINDOWS ")
                SET_TARGET_PROPERTIES(${APPLICATION_EXE_NAME} PROPERTIES MINSIZEREL "/WINMD /SUBSYSTEM:WINDOWS /OPT:REF /map Ws2_32.lib WindowsApp.lib /APPCONTAINER  /DYNAMICBASE")
        endif()


TARGET_LINK_LIBRARIES(${APPLICATION_EXE_NAME}
        ${libs}
        Qt5::Widgets
        Qt5::Core
        ${QT_QTGUI_LIBRARY}
        ${PLEXY_UI_CORE_LIBRARY}
        ${Qt5X11Extras_LIBRARIES}
        ${MAC_LIBS}
        )

QT5_USE_MODULES(${APPLICATION_EXE_NAME} Widgets Xml Core)

########### Install files ###############
if(UNIX)
  install(TARGETS plexydesk DESTINATION bin)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/plexydesk.desktop DESTINATION share/applications)
endif()

if(WIN32)
  install(TARGETS plexydesk RUNTIME DESTINATION .)
  INSTALL(FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
                                        DESTINATION . COMPONENT Libraries)
endif()



if (WIN32)
get_target_property(QtCore_LOCATION Qt5::Core LOCATION)
get_filename_component(QT_DLL_DIR ${QtCore_LOCATION} PATH)


install(FILES
  ${QT_DLL_DIR}/Qt5Core.dll
  ${QT_DLL_DIR}/Qt5Gui.dll
  ${QT_DLL_DIR}/Qt5Widgets.dll
  ${QT_DLL_DIR}/libEGL.dll
  ${QT_DLL_DIR}/libGLESV2.dll
  ${QT_DLL_DIR}/Qt5Multimedia.dll
  ${QT_DLL_DIR}/Qt5Svg.dll
  ${QT_DLL_DIR}/Qt5Xml.dll
  ${QT_DLL_DIR}/Qt5PrintSupport.dll
  ${QT_DLL_DIR}/Qt5Network.dll
  ${QT_DLL_DIR}/Qt5OpenGL.dll
  DESTINATION .
)

if (NOT ENABLE_WINRT)
	install(FILES
  		${QT_DLL_DIR}/opengl32sw.dll 
  		${QT_DLL_DIR}/D3Dcompiler_47.dll
		DESTINATION .)


	install(FILES
  		${QT_DLL_DIR}/../plugins/audio/qtaudio_windows.dll
  		DESTINATION audio)

	install(FILES
  		${QT_DLL_DIR}/../plugins/mediaservice/dsengine.dll
  		${QT_DLL_DIR}/../plugins/mediaservice/qtmedia_audioengine.dll
  		${QT_DLL_DIR}/../plugins/mediaservice/wmfengine.dll
		DESTINATION mediaservice)

	install(FILES
  		${QT_DLL_DIR}/../plugins/platforms/qwindows.dll
		DESTINATION platforms
		)


	install(FILES
  		${QT_DLL_DIR}/../plugins/printsupport/windowsprintersupport.dll
  		DESTINATION printsupport
		)

	install(FILES
  		${QT_DLL_DIR}/../plugins/bearer/qgenericbearer.dll
  		${QT_DLL_DIR}/../plugins/bearer/qnativewifibearer.dll
  		DESTINATION bearer
		)
endif()

if (NOT ENABLE_WINRT)
	install(FILES
  		${QT_DLL_DIR}/../plugins/imageformats/qtga.dll
  		${QT_DLL_DIR}/../plugins/imageformats/qwebp.dll
		DESTINATION platforms
		)
endif()

if (ENABLE_WINRT)
	install(FILES
  		${QT_DLL_DIR}/../plugins/platforms/qwinrt.dll
		DESTINATION platforms
		)
endif()


install(FILES
  ${QT_DLL_DIR}/../plugins/imageformats/qicns.dll
  ${QT_DLL_DIR}/../plugins/imageformats/qwbmp.dll
  ${QT_DLL_DIR}/../plugins/imageformats/qsvg.dll
  ${QT_DLL_DIR}/../plugins/imageformats/qjpeg.dll
  ${QT_DLL_DIR}/../plugins/imageformats/qico.dll
  ${QT_DLL_DIR}/../plugins/imageformats/qgif.dll

  DESTINATION imageformats
)

install(FILES
  ${QT_DLL_DIR}/../plugins/iconengines/qsvgicon.dll

  DESTINATION iconengines
)

install(FILES
  ${QT_DLL_DIR}/../plugins/bearer/qgenericbearer.dll
  DESTINATION bearer
)

install(FILES
  ${QT_DLL_DIR}/../plugins/playlistformats/qtmultimedia_m3u.dll

  DESTINATION playlistformats
)
install(FILES
  ${QT_DLL_DIR}/../translations/qt_ca.qm
  ${QT_DLL_DIR}/../translations/qt_cs.qm
  ${QT_DLL_DIR}/../translations/qt_de.qm
  ${QT_DLL_DIR}/../translations/qt_fi.qm
  ${QT_DLL_DIR}/../translations/qt_fr.qm
  ${QT_DLL_DIR}/../translations/qt_hu.qm
  ${QT_DLL_DIR}/../translations/qt_it.qm
  ${QT_DLL_DIR}/../translations/qt_ja.qm
  ${QT_DLL_DIR}/../translations/qt_ko.qm
  ${QT_DLL_DIR}/../translations/qt_lt.qm
  ${QT_DLL_DIR}/../translations/qt_ru.qm
  ${QT_DLL_DIR}/../translations/qt_sk.qm
  ${QT_DLL_DIR}/../translations/qt_uk.qm

  DESTINATION translations
)

include(${CMAKE_SOURCE_DIR}/cmakemodules/windows-code-sign.cmake)

endif()

if (ENABLE_WINRT)
 set(APPX_PUBLISHER "PlexyDesk")
 set(APPX_VERSION "1.0.3.0")
 set(APPX_ARCH "x64")
 set(APPX_BINARY "plexydesk")
 set(APPX_DEVICE_FAMILY "Windows.Universal")
 set(APPX_PRODUCT_DISPLAY_NAME "PlexyDesk Cherry")
 set(APPX_PRODUCT_DESCRIPTION "PlexyDesk Desktop Manager")

 configure_file(${CMAKE_SOURCE_DIR}/dist/windows/appstore/AppxManifest.xml.in ${CMAKE_BINARY_DIR}/AppxManifest.xml)

 install(FILES
 	${CMAKE_BINARY_DIR}/AppxManifest.xml
  	DESTINATION .)

  #install artwork
  install(FILES assets/appstore-logo.scale44.png DESTINATION assets)
  install(FILES assets/appstore-logo.scale150.png DESTINATION assets)
  install(FILES assets/appstore-logo.scale400.png DESTINATION assets)

endif()
