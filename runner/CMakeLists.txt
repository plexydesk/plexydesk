set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP true)
include(InstallRequiredSystemLibraries)

if(UNIX)
  INCLUDE_DIRECTORIES(
    ${X11_INCLUDE_DIR}
    ${Qt5X11Extras_INCLUDE_DIRS}
    ${QT_QTGUI_INCLUDE_DIR}
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS})

  set(extra_libs Xcomposite Xdamage)
endif(UNIX)

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
  ADD_DEFINITIONS(-DNODEFAULTLIB:library)
  set(libs
    ${QT_QTGUI_LIBRARY}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTMAIN_LIBRARY}
    Qt5::WinMain
    runtimeobject.lib
    windowsapp.lib
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
message("Building for windows 32 ${APPLICATION_EXE_NAME} - ${WIN_APPLICATION}")
ADD_EXECUTABLE(${APPLICATION_EXE_NAME} ${APP_TYPE} ${WIN_APPLICATION} ${sourceFiles} ${resourceFiles} ${QT_MOC_SRCS})
set_property(TARGET ${APPLICATION_EXE_NAME} PROPERTY VS_WINRT_COMPONENT TRUE)

#if(MSVC)
#	MSVC_SET_PROPER_CONSOLE_HANDLING(${APPLICATION_EXE_NAME})
#endif(MSVC)
#SET_PROPERTY(TARGET ${APPLICATION_EXE_NAME} PROPERTY QT5_NO_LINK_QTMAIN ON)

IF(WIN32)
SET_TARGET_PROPERTIES(${APPLICATION_EXE_NAME} PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
SET_TARGET_PROPERTIES(${APPLICATION_EXE_NAME} PROPERTIES MINSIZEREL "/SUBSYSTEM:WINDOWS")
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
  		${QT_DLL_DIR}/../plugins/imageformats/qdds.dll
  		DESTINATION imageformats
		)

	install(FILES
  		${QT_DLL_DIR}/../plugins/bearer/qgenericbearer.dll
  		${QT_DLL_DIR}/../plugins/bearer/qnativewifibearer.dll
  		DESTINATION bearer
		)
endif()

if (NOT ENABLE_WINRT)
	install(FILES
  		${QT_DLL_DIR}/../plugins/imageformats/qjp2.dll
  		${QT_DLL_DIR}/../plugins/imageformats/qtga.dll
  		${QT_DLL_DIR}/../plugins/imageformats/qmng.dll
  		${QT_DLL_DIR}/../plugins/imageformats/qwebp.dll
  		${QT_DLL_DIR}/../plugins/imageformats/qtiff.dll
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


if (NOT ENABLE_WINRT)
	windows_code_sign_deps(opengl32sw.dll)
	windows_code_sign_deps(libEGL.dll)
	windows_code_sign_deps(audio/qtaudio_windows.dll)
	windows_code_sign_deps(printsupport/windowsprintersupport.dll)
	windows_code_sign_deps(imageformats/qgif.dll)
	windows_code_sign_deps(D3Dcompiler_47.dll)
	windows_code_sign_deps(libGLESV2.dll)
	windows_code_sign_deps(Qt5Multimedia.dll)
	windows_code_sign_deps(mediaservice/dsengine.dll)
	windows_code_sign_deps(mediaservice/qtmedia_audioengine.dll)
	windows_code_sign_deps(imageformats/qdds.dll)
	windows_code_sign_deps(imageformats/qwebp.dll)
	windows_code_sign_deps(imageformats/qtga.dll)
	windows_code_sign_deps(imageformats/qsvg.dll)
	windows_code_sign_deps(bearer/qnativewifibearer.dll)
	windows_code_sign_deps(platforms/qwindows.dll)
	windows_code_sign_deps(mediaservice/wmfengine.dll)
	windows_code_sign_deps(imageformats/qmng.dll)
	windows_code_sign_deps(imageformats/qtiff.dll)
	windows_code_sign_deps(imageformats/qjp2.dll)
endif()


windows_code_sign_deps(Qt5Core.dll)
windows_code_sign_deps(Qt5Widgets.dll)
windows_code_sign_deps(Qt5Gui.dll)
windows_code_sign_deps(Qt5Svg.dll)
windows_code_sign_deps(Qt5Xml.dll)
windows_code_sign_deps(Qt5OpenGL.dll)
windows_code_sign_deps(Qt5Network.dll)
windows_code_sign_deps(iconengines/qsvgicon.dll)
windows_code_sign_deps(bearer/qgenericbearer.dll)
windows_code_sign_deps(imageformats/qicns.dll)
windows_code_sign_deps(imageformats/qwbmp.dll)
windows_code_sign_deps(imageformats/qjpeg.dll)
windows_code_sign_deps(imageformats/qico.dll)
windows_code_sign_deps(playlistformats/qtmultimedia_m3u.dll)
windows_code_sign_deps(platforms/qwinrt.dll)
windows_code_sign_target(${APPLICATION_EXE_NAME} exe)

endif()
