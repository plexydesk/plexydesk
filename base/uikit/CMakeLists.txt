#UiKit
#siraj@plexydesk.com

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
    core/ck_timer.cpp
    core/ck_screen.cpp
    core/ck_widget.cpp
    core/ck_ui_action.cpp
    core/ck_desktop_controller_interface.cpp
    core/ck_data_source.cpp
    core/ck_desktop_dialog.cpp
    core/ck_extension_manager.cpp
    core/ck_style.cpp)

if(APPLE)
  set(CORE_LIB_CXX_FILES ${CORE_LIB_CXX_FILES}
       core/mac/ck_screen_impl_mac.mm
      )
endif()

if(UNIX AND NOT APPLE)
  set(CORE_LIB_CXX_FILES ${CORE_LIB_CXX_FILES}
       core/qt/ck_screen_impl_qt.cpp
      )
endif()

if(WIN32)
  set(CORE_LIB_CXX_FILES ${CORE_LIB_CXX_FILES}
       core/qt/ck_screen_impl_qt.cpp
      )
endif()


set(CORE_LIBRARY_INTERFACE_FILES
    core/ck_desktop_controller_interface.h
    core/ck_desktop_plugin_interface.h
    core/ck_data_plugin_interface.h
    core/ck_dialog_plugin_interface.h
    core/ck_style_interface.h)

set(CORE_LIBRARY_HEADER_FILES
    core/ck_timer.h
    core/ck_style.h
    core/ck_screen.h
    core/ck_widget.h
    core/ck_data_source.h
    core/ck_style_data.h
    core/ck_desktop_dialog.h
    core/ck_extension_manager.h
    core/ck_ui_action.h)
#
set(LIBRARY_WIDGET_SOURCE_CXX_FILES
    widgets/ck_button.cpp
    widgets/ck_scroll_widget.cpp
    widgets/ck_image_view.cpp
    widgets/ck_label.cpp
    widgets/ck_progress_bar.cpp
    widgets/ck_icon_button.cpp
    widgets/ck_button.cpp
    widgets/ck_line_edit.cpp
    widgets/ck_text_editor.cpp
    widgets/ck_window_button.cpp
    widgets/ck_table_model.cpp
    widgets/ck_table_view_item.cpp
    widgets/ck_item_view.cpp
    widgets/ck_scrollbar.cpp
    components/ck_dial_view.cpp
    components/ck_clock_view.cpp
    components/ck_range_view.cpp
    components/ck_model_view_item.cpp
    components/ck_calendar_view.cpp
    ck_fixed_layout.cpp
    )

if (WIN32)
  set(PLATFORM_SOURCE_FILES
    platform/ck_win32_window_context.cpp
  )
  set(PLATFORM_HEADER_FILES
    platform/ck_win32_window_context.h
  )
endif()

set(LIBRARY_SOURCE_FILES
    netwm.cpp
    ck_fixed_layout.cpp
    ck_resource_manager.cpp
    ck_config.cpp
    ck_window.cpp
    ck_workspace.cpp
    ck_session_sync.cpp
    ck_space.cpp
    ck_device_window.cpp
    ck_system_window_context.cpp
    ${PLATFORM_SOURCE_FILES}
    ${LIBRARY_WIDGET_SOURCE_CXX_FILES}
    ${LIBRARY_PLATFORM_SOURCE_CXX_FILES})

set(LIBRARY_HEADER_FILES
    ck_fixed_layout.h
    ck_workspace.h
    netwm.h
    netwm_def.h
    netwm_p.h
    ck_resource_manager.h
    ck_window.h
    ck_config.h
    widgets/ck_button.h
    widgets/ck_line_edit.h
    widgets/ck_text_editor.h
    widgets/ck_window_button.h
    widgets/ck_scroll_widget.h
    widgets/ck_image_view.h
    widgets/ck_label.h
    widgets/ck_progress_bar.h
    widgets/ck_icon_button.h
    widgets/ck_table_model.h
    widgets/ck_table_view_item.h
    widgets/ck_item_view.h
    widgets/ck_scrollbar.h
    components/ck_dial_view.h
    components/ck_clock_view.h
    components/ck_range_view.h
    components/ck_model_view_item.h
    components/ck_calendar_view.h
    ck_fixed_layout.h
    ck_workspace.h
    ck_session_sync.h
    ck_space.h
    ck_device_window.h
    ck_system_window_context.h
    ${PLATFORM_HEADER_FILES}
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

    qt5_use_modules(${PLEXY_UI_CORE_LIBRARY} Gui Widgets Xml OpenGL  Core)

if(UNIX AND NOT APPLE)
    set_target_properties(${PLEXY_UI_CORE_LIBRARY} PROPERTIES SOVERSION "1.0.0")
    install(TARGETS ${PLEXY_UI_CORE_LIBRARY} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} NAMELINK_SKIP)
endif()

if (APPLE)
    set_target_properties(${PLEXY_UI_CORE_LIBRARY} PROPERTIES SKIP_BUILD_RPATH 0)
    install(TARGETS ${PLEXY_UI_CORE_LIBRARY}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        FRAMEWORK DESTINATION plexydesk.app/Contents/Frameworks)
endif(APPLE)

if(WIN32)
    # installs to the same path as the binary
    install(TARGETS ${PLEXY_UI_CORE_LIBRARY} DESTINATION ${CMAKE_INSTALL_PREFIX})
endif()
