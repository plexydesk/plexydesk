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
    Qt5::WinMain)
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
  install(TARGETS plexydesk RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
endif()
