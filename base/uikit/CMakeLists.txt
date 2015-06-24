#UiKit
#siraj@plexydesk.org

cmake_policy(SET CMP0043 NEW)

set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTOUIC ON) // currently we are not using any .ui files

include (GenerateExportHeader)

if(NOT CMAKE_INSTALL_LIBDIR)
    include(GNUInstallDirs)
endif(NOT CMAKE_INSTALL_LIBDIR)

if(NOT QNX)
    set(LIBRARY_PLATFORM_HEADERS)
    set(LIBRARY_PLATFORM_SOURCE_CXX_FILES)
endif (NOT QNX)

#todo: check on mac and remove it.
#if(APPLE)
#    set(LIBRARY_PLATFORM_SOURCE_CXX_FILES qtscroller_mac.mm)
#endif(APPLE)

if(UNIX AND NOT APPLE)
    include_directories(
        ${X11_INCLUDE_DIR}
        ${Qt5X11Extras_INCLUDE_DIRS}
        ${QT_QTGUI_INCLUDE_DIR}
        ${Qt5Gui_PRIVATE_INCLUDE_DIRS})
endif(UNIX AND NOT APPLE)

#stage 1 : move core files
set(CORE_LIB_CXX_FILES
    core/widget.cpp
    core/controller_action.cpp
    core/view_controller.cpp
    core/datasource.cpp
    core/desktopactivity.cpp
    core/extensionmanager.cpp
    core/style.cpp)

set(CORE_LIBRARY_INTERFACE_FILES
    core/view_controller.h
    core/controllerplugininterface.h
    core/dataplugininterface.h
    core/activity_interface.h
    core/widgetstyleinterface.h)

set(CORE_LIBRARY_HEADER_FILES
    core/plexy.h
    core/style.h
    core/widget.h
    core/datasource.h
    core/stylefeatures.h
    core/desktopactivity.h
    core/extensionmanager.h
    core/controller_action.h)

#
set(LIBRARY_WIDGET_SOURCE_CXX_FILES
    widgets/button.cpp
    widgets/scrollwidget.cpp
    widgets/imageview.cpp
    widgets/label.cpp
    widgets/progressbar.cpp
    widgets/imagebutton.cpp
    widgets/default_table_model.cpp
    widgets/default_table_component.cpp
    widgets/button.cpp
    widgets/lineedit.cpp
    widgets/texteditor.cpp
    widgets/windowbutton.cpp
    widgets/tableview.cpp
    widgets/tableviewcellinterface.cpp
    widgets/abstractcellcomponent.cpp
    widgets/toolbar.cpp
    widgets/item_view.cpp
    components/dialwidget.cpp
    components/clockwidget.cpp
    components/rangewidget.cpp
    components/model_view_item.cpp
    components/calendarwidget.cpp
    viewbuilder.cpp
    )

set(LIBRARY_SOURCE_FILES
    netwm.cpp
    viewbuilder.cpp
		resource_manager.cpp
    plexyconfig.cpp
    window.cpp
    workspace.cpp
    session_sync.cpp
    space.cpp
    ${LIBRARY_WIDGET_SOURCE_CXX_FILES}
    ${LIBRARY_PLATFORM_SOURCE_CXX_FILES})

set(LIBRARY_HEADER_FILES
    viewbuilder.h
    workspace.h
    netwm.h
    netwm_def.h
    netwm_p.h
    resource_manager.h
    window.h
    plexyconfig.h
    widgets/button.h
    widgets/lineedit.h
    widgets/texteditor.h
    widgets/windowbutton.h
    widgets/tableview.h
    widgets/scrollwidget.h
    widgets/imageview.h
    widgets/label.h
    widgets/progressbar.h
    widgets/imagebutton.h
    widgets/default_table_model.h
    widgets/default_table_component.h
    widgets/tableviewcellinterface.h
    widgets/abstractcellcomponent.h
    widgets/toolbar.h
    widgets/item_view.h
    components/dialwidget.h
    components/clockwidget.h
    components/rangewidget.h
    components/model_view_item.h
    components/calendarwidget.h
    viewbuilder.h
    workspace.h
    session_sync.h
    space.h
    ${CORE_LIBRARY_HEADER_FILES}
    ${CORE_LIBRARY_INTERFACE_FILES}
    ${LIBRARY_PLATFORM_HEADERS}
    )

#Locate cococa framework on mac
if(APPLE)
    include_directories(/System/Library/Frameworks)

    find_library(COCOA_LIBRARY Cocoa)
    find_library(GLUT_LIBRARY GLUT)
    find_library(OpenGL_LIBRARY OpenGL)

    mark_as_advanced(COCOA_LIBRARY
        GLUT_LIBRARY
        OpenGL_LIBRARY)

    set(LIBRARY_NATIVE_LINK_TARGETS
        ${COCOA_LIBRARY}
        ${GLUT_LIBRARY}
        ${OpenGL_LIBRARY})
endif(APPLE)

set(LIBRARY_SOURCE_FILES
    ${LIBRARY_SOURCE_FILES}
    ${LIBRARY_HEADER_FILES}
    )

if (NOT WIN32)
  set (THREAD_LIBRARY pthread) 
endif()

set(LIBRARY_LINK_TARGETS
    ${THREAD_LIBRARY}
    ${OPENGL_LIBRARIES}
    ${QUETZAL_DATA_KIT_LIBRARY}
    ${QUETZAL_SOCIAL_KIT_LIBRARY})

#qt5_wrap_ui(UI_FILES ${uiFiles})

add_library(${PLEXY_UI_CORE_LIBRARY} SHARED
    ${LIBRARY_SOURCE_FILES}
    ${CORE_LIB_CXX_FILES}
    ${UI_FILES})

if(APPLE)
    set_target_properties(${PLEXY_UI_CORE_LIBRARY} PROPERTIES FRAMEWORK 1)
endif(APPLE)

generate_export_header(${PLEXY_UI_CORE_LIBRARY}
    BASE_NAME ${PLEXY_UI_CORE_LIBRARY}
    EXPORT_MACRO_NAME DECL_UI_KIT_EXPORT
    EXPORT_FILE_NAME plexydesk_ui_exports.h
    STATIC_DEFINE ${PLEXY_UI_CORE_LIBRARY}_BUILT_AS_STATIC)

if(MINGW OR WIN32)
    set_target_properties(${PLEXY_UI_CORE_LIBRARY} PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX "")
endif(MINGW OR WIN32)

target_link_libraries(${PLEXY_UI_CORE_LIBRARY}
    ${LIBRARY_LINK_TARGETS}
    ${LIBRARY_NATIVE_LINK_TARGETS})

qt5_use_modules(${PLEXY_UI_CORE_LIBRARY} Widgets Xml OpenGL Core)

if(UNIX AND NOT APPLE)
    set_target_properties(${PLEXY_UI_CORE_LIBRARY} PROPERTIES SOVERSION "1.0.0")
    install(TARGETS ${PLEXY_UI_CORE_LIBRARY} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} NAMELINK_SKIP)
endif()

if (APPLE)
    set_target_properties(${PLEXY_UI_CORE_LIBRARY} PROPERTIES SKIP_BUILD_RPATH 1)
    install(TARGETS ${PLEXY_UI_CORE_LIBRARY}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        FRAMEWORK DESTINATION plexydesk.app/Contents/Frameworks)
endif(APPLE)

if(WIN32)
    # installs to the same path as the binary
    install(TARGETS ${PLEXY_UI_CORE_LIBRARY} DESTINATION ${CMAKE_INSTALL_PREFIX})
endif()
