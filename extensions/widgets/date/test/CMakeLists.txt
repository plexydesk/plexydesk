set(CMAKE_AUTOMOC ON)

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer -O0 -g")
    endif()
endif()

########### Source files ###############
set(sourceFiles
    ../event_store.cpp
    event_sync.cc
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

set(libs ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY}
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
    ${THREAD_LIBS}
    ${X11_LIBRARIES}
    ${LIBRARY_NATIVE_LINK_TARGETS}
    )


########### prepare files ###############
ADD_EXECUTABLE(event_tool ${WIN_APPLICATION} ${sourceFiles})

#target properties for OS X
if(APPLE)
    set_target_properties(event_tool PROPERTIES SKIP_BUILD_RPATH FALSE)
    set_target_properties(event_tool PROPERTIES SKIP_INSTALL_RPATH FALSE)
    set(CMAKE_INSTALL_RPATH "@executable_path/../Frameworks/")
endif()

#SET_PROPERTY(TARGET event_tool PROPERTY QT5_NO_LINK_QTMAIN ON)
#QT5_USE_MODULES(event_tool Widgets Xml  Core)

if (MSVC)
    MSVC_SET_PROPER_CONSOLE_HANDLING(event_tool)
endif(MSVC)

TARGET_LINK_LIBRARIES(event_tool LINK_PUBLIC
    ${QUETZAL_DATA_KIT_LIBRARY}
    ${libs}
    )

