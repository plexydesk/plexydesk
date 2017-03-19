set(CMAKE_AUTOMOC ON)

	if (CMAKE_BUILD_TYPE MATCHES "Debug")
	if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer -O0 -g")
	endif()
endif()

########### Source files ###############
	set(sourceFiles
			main.cpp
	   )

	if (WIN32)
	set(GUI_TYPE WIN32)
	ADD_DEFINITIONS(-DNODEFAULTLIB:library)
endif(WIN32)

	if (UNIX AND NOT APPLE)
	set (THREAD_LIBS pthread)
endif()

	if(UNIX AND NOT APPLE)
if(THREADS_HAVE_PTHREAD_ARG)
	target_compile_options(PUBLIC ${PLEXY_UI_CORE_LIBRARY} "-pthread")
endif()

	set(LIBRARY_NATIVE_LINK_TARGETS
			${X11_LIBRARIES}
			${THREAD_LIBRARY}
			${CMAKE_THREAD_LIBS_INIT}
	   )
endif()

if (ENABLE_QT5)
	set(libs ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY}
			${QT_QTOPENGL_LIBRARY}
			${QT_QTSVG_LIBRARY}
			${QT_QTNETWORK_LIBRARY}
			${QT_QTDECLARATIVE_LIBRARY}
			${QT_QTSQL_LIBRARY}
			${QT_QTSCRIPT_LIBRARY}
			${QT_QTXML_LIBRARY}
			${QT_QTXMLPATTERNS_LIBRARY}
			${Qt5X11Extras_LIBRARIES}
			${THREAD_LIBS}
			${X11_LIBRARIES}
			${LIBRARY_NATIVE_LINK_TARGETS}
	   )
endif()


########### prepare files ###############
ADD_EXECUTABLE(datasync_runtime ${WIN_APPLICATION} ${sourceFiles})

#SET_PROPERTY(TARGET datasync_runtime PROPERTY QT5_NO_LINK_QTMAIN ON)

	if (MSVC)
	MSVC_SET_PROPER_CONSOLE_HANDLING(datasync_runtime)
endif(MSVC)

if (ENABLE_QT4)
	TARGET_LINK_LIBRARIES(datasync_runtime LINK_PUBLIC
			${QUETZAL_DATA_KIT_LIBRARY}
			${libs}
			Qt4::QtGui Qt4::QtXml
			)
endif()

if (ENABLE_QT5)
	TARGET_LINK_LIBRARIES(datasync_runtime LINK_PUBLIC
			${QUETZAL_DATA_KIT_LIBRARY}
			${libs}
			)
endif()


########### Install files ###############
if(APPLE)
	install(TARGETS datasync_runtime
			BUNDLE DESTINATION . COMPONENT Runtime
			RUNTIME DESTINATION bin COMPONENT Runtime)
endif(APPLE)

	if (NOT APPLE)
	INSTALL(TARGETS datasync_runtime DESTINATION bin)
endif()
