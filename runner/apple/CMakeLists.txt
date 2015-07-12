if (NOT APPLE)
	IF(UNIX)
		INCLUDE_DIRECTORIES(
			${X11_INCLUDE_DIR}
			${Qt5X11Extras_INCLUDE_DIRS}
			${QT_QTGUI_INCLUDE_DIR}
			${Qt5Gui_PRIVATE_INCLUDE_DIRS}
			)

		set(extra_headers)

		set(extra_source)

		set(extra_libs
			Xcomposite
			Xdamage
			)
	endif(UNIX)
	IF(WIN32)
		INCLUDE_DIRECTORIES(
			${CMAKE_SOURCE_DIR}/dist/windows
			${CMAKE_BINARY_DIR}/dist/windows
			)
		set(resourceFiles
			${CMAKE_BINARY_DIR}/dist/windows/${APPLICATION_EXE_NAME}_windres.rc
			)
	endif(WIN32)
endif(NOT APPLE)

########### Source files ###############
set(sourceFiles
	${extra_source}
	)


if(APPLE)
	set(mainFiles main.mm)
	set(OSX_ICON_FILES ${CMAKE_SOURCE_DIR}/artwork/icons/plexydesk.icns)
	set(MACOSX_BUNDLE_ICON_FILE "plexydesk.icns")
endif(APPLE)

if (NOT APPLE)
	set(mainFiles main.cpp
		)
endif(NOT APPLE)

set(headerFiles ../desktopmanager.h
	${extra_headers}
	)

set(QTMOC_SRCS ../desktopmanager.h
	${extra_headers}
	)

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

set(sourceFiles
	../desktopmanager.cpp
	${sourceFiles}
	${headerFiles}
	${mainFiles}
	)

if (WIN32)
	set(GUI_TYPE WIN32)
	ADD_DEFINITIONS(-DNODEFAULTLIB:library)
	set(libs
		${QT_QTGUI_LIBRARY}
		${QT_QTCORE_LIBRARY}
		${QT_QTOPENGL_LIBRARY}
		${QT_QTMAIN_LIBRARY}
		)
endif(WIN32)

if (UNIX)
	set(libs
		${QT_QTGUI_LIBRARY}
		${QT_QTCORE_LIBRARY}
		${QT_QTOPENGL_LIBRARY}
		${QT_QTSQL_LIBRARY}
		${Qt5X11Extras_LIBRARIES}
		${extra_libs}
		)
endif(UNIX)

#Locate cococa framework on mac
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

########### Prepare files ###############

if (APPLE)
	set (APP_TYPE MACOSX_BUNDLE)
endif(APPLE)

ADD_EXECUTABLE(${APPLICATION_EXE_NAME} ${APP_TYPE} ${WIN_APPLICATION} ${sourceFiles} ${resourceFiles} ${QT_MOC_SRCS})
if (APPLE)
	set(MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME})
	SET_TARGET_PROPERTIES(${APPLICATION_EXE_NAME} PROPERTIES INSTALL_RPATH "@executable_path/../Frameworks/")
	SET_TARGET_PROPERTIES(${APPLICATION_EXE_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/cmakemodules/plexydesk-info.plist)
	#rpath specific properties
	message (${APPLICATION_EXE_NAME})
	set(CMAKE_INSTALL_RPATH "@executable_path/../Frameworks/")

endif(APPLE)

if (MSVC)
	MSVC_SET_PROPER_CONSOLE_HANDLING(${APPLICATION_EXE_NAME})
endif(MSVC)
SET_PROPERTY(TARGET ${APPLICATION_EXE_NAME} PROPERTY QT5_NO_LINK_QTMAIN ON)
TARGET_LINK_LIBRARIES(${APPLICATION_EXE_NAME} ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY}
	${QT_QTOPENGL_LIBRARY}
	${QT_QTSVG_LIBRARY}
	${QT_QTNETWORK_LIBRARY}
	${QT_QTDECLARATIVE_LIBRARY}
	${QT_QTSQL_LIBRARY}
	${QT_QTSCRIPT_LIBRARY}
	${QT_QTXML_LIBRARY}
	${PLEXY_UI_CORE_LIBRARY}
	${QT_QTXMLPATTERNS_LIBRARY}
	${Qt5X11Extras_LIBRARIES}
	${MAC_LIBS}
	)

QT5_USE_MODULES(${APPLICATION_EXE_NAME} Widgets Xml OpenGL Core  )

########### Install files ###############
if (NOT APPLE)
	INSTALL(TARGETS plexydesk DESTINATION bin)
endif(NOT APPLE)

if (APPLE)
	INSTALL(TARGETS plexydesk
		BUNDLE DESTINATION . COMPONENT Runtime
		RUNTIME DESTINATION bin COMPONENT Runtime)
	INSTALL(FILES ${OSX_ICON_FILES}
		DESTINATION plexydesk.app/Contents/Resources)
endif(APPLE)


if (UNIX)
	iF (NOT APPLE)
		INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/plexydesk.desktop DESTINATION share/applications)
	endif(NOT APPLE)
endif(UNIX)
