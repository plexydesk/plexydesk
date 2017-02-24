set(CMAKE_AUTOMOC ON)

########### Source files ###############
set(sourceFiles
    main.cpp
    socialtestrunner.cpp
    test_url_post_request.cpp
    )

if (WIN32)
    set(GUI_TYPE WIN32)
    ADD_DEFINITIONS(-DNODEFAULTLIB:library)
endif(WIN32)

if (ENABLE_WINRT)
    set(winrt_libs WindowsApp.lib)
endif()

set(libs ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY}
    ${QT_QTOPENGL_LIBRARY}
    ${QT_QTSVG_LIBRARY}
    ${QT_QTNETWORK_LIBRARY}
    ${QT_QTSQL_LIBRARY}
    ${QT_QTSCRIPT_LIBRARY}
    ${QT_QTXML_LIBRARY}
    ${PLEXY_UI_CORE_LIBRARY}
    ${QT_QTXMLPATTERNS_LIBRARY}
		${X11_LIBRARIES}
    ${Qt5X11Extras_LIBRARIES}
    ${winrt_libs}
    )


########### prepare files ###############


ADD_EXECUTABLE(social_runtime WIN32 ${WIN_APPLICATION} ${sourceFiles})


if (ENABLE_WINRT)
                SET_TARGET_PROPERTIES(social_runtime PROPERTIES LINK_FLAGS_RELEASE "/WINMD /OPT:REF /map Ws2_32.lib WindowsApp.lib /APPCONTAINER  /DYNAMICBASE /SUBSYSTEM:WINDOWS ")
                SET_TARGET_PROPERTIES(social_runtime PROPERTIES MINSIZEREL "/WINMD /SUBSYSTEM:WINDOWS /OPT:REF /map Ws2_32.lib WindowsApp.lib /APPCONTAINER  /DYNAMICBASE")
		set_property(TARGET social_runtime PROPERTY VS_WINRT_COMPONENT TRUE)
endif()


#SET_PROPERTY(TARGET social_runtime PROPERTY QT5_NO_LINK_QTMAIN ON)

QT5_USE_MODULES(social_runtime Widgets Xml  Core)

if (MSVC)
    MSVC_SET_PROPER_CONSOLE_HANDLING(social_runtime)
endif(MSVC)

if (ENABLE_WINRT)
      set(winrt_libs windowsapp.lib)
endif()



TARGET_LINK_LIBRARIES(social_runtime
    ${PLEXY_UI_CORE_LIBRARY}
    ${QUETZAL_SOCIAL_KIT_LIBRARY}
    ${QT_QTMAIN_LIBRARY}
    ${libs}
    ${winrt_libs}
    )

 #DBUS_ADD_ACTIVATION_SERVICE(org.plexydesk.social.service.in)

########### Install files ###############
if(UNIX)
INSTALL(TARGETS social_runtime DESTINATION bin)
endif(UNIX)

if (ENABLE_WINRT)
 set(APPX_PUBLISHER "PlexyDesk Social Runtime")
 set(APPX_VERSION "1.0.3.0")
 set(APPX_ARCH "x64")
 set(APPX_BINARY "social_runtime")
 set(APPX_DEVICE_FAMILY "Windows.Universal")
 set(APPX_PRODUCT_DISPLAY_NAME "Social Runtime")
 set(APPX_PRODUCT_DESCRIPTION "Social Manager")

 configure_file(${CMAKE_SOURCE_DIR}/base/social/testrunner/AppxManifest.xml.in ${CMAKE_BINARY_DIR}/base/social/testrunner/AppxManifest.xml)

 install(FILES
 	${CMAKE_BINARY_DIR}/base/social/testrunner/AppxManifest.xml
  	DESTINATION social)

  #install artwork
  INSTALL(TARGETS social_runtime DESTINATION social)

endif()
