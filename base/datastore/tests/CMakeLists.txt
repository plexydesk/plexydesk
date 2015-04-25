set(CMAKE_AUTOMOC ON)
########### Source files ###############
SET(sourceFiles
    main.cpp
    datasyncruntime.cpp
    )

IF(WIN32)
    SET(GUI_TYPE WIN32)
    ADD_DEFINITIONS(-DNODEFAULTLIB:library)
ENDIF(WIN32)

SET(libs ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY}
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
    )


########### prepare files ###############
ADD_EXECUTABLE(datasync_runtime ${WIN_APPLICATION} ${sourceFiles})

#target properties for OS X
if(APPLE)
    set_target_properties(datasync_runtime PROPERTIES SKIP_BUILD_RPATH FALSE)
    set_target_properties(datasync_runtime PROPERTIES SKIP_INSTALL_RPATH FALSE)
    set(CMAKE_INSTALL_RPATH "@executable_path/../Frameworks/")
endif()

#SET_PROPERTY(TARGET datasync_runtime PROPERTY QT5_NO_LINK_QTMAIN ON)
#QT5_USE_MODULES(datasync_runtime Widgets Xml OpenGL Core)

IF(MSVC)
    MSVC_SET_PROPER_CONSOLE_HANDLING(datasync_runtime)
ENDIF(MSVC)

TARGET_LINK_LIBRARIES(datasync_runtime LINK_PUBLIC
    ${QUETZAL_DATA_KIT_LIBRARY}
    )

########### Install files ###############
if(APPLE)
   install(TARGETS datasync_runtime
                            BUNDLE DESTINATION . COMPONENT Runtime
                            RUNTIME DESTINATION bin COMPONENT Runtime)
endif(APPLE)

if (NOT APPLE)
  INSTALL(TARGETS datasync_runtime DESTINATION bin)
endif()
