cmake_minimum_required(VERSION 3.0.1)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

if(NOT CMAKE_INSTALL_LIBDIR)
    include(GNUInstallDirs)
endif(NOT CMAKE_INSTALL_LIBDIR)

include(GenerateExportHeader)

set(LIB_CXX_FILES
                widget.cpp
                controller_action.cpp
		ck_desktop_controller_interface.cpp
                datasource.cpp
                ck_desktop_dialog.cpp
                ck_extension_manager.cpp
                style.cpp
                desktop_viewport.cpp)

set(LIBRARY_INTERFACE_FILES
	        ck_desktop_controller_interface.h
                controllerplugininterface.h
                ck_data_plugin_interface.h
                ck_dialog_plugin_interface.h
                ck_style_interface.h)

set(LIBRARY_HEADER_FILES
                plexy.h
                style.h
                ck_widget.h
                ck_data_source.h
                ck_style_data.h
                ck_desktop_dialog.h
                desktop_viewport.h
                ck_extension_manager.h
                controller_action.h)

set(LIBRARY_LINK_TARGETS
                         ${OPENGL_LIBRARIES}
                         ${QUETZAL_DATA_KIT_LIBRARY}
                         ${QUETZAL_SOCIAL_KIT_LIBRARY})


add_library(${PLEXY_CORE_LIBRARY} SHARED
                                        ${LIB_CXX_FILES}
                                        ${LIBRARY_INTERFACE_FILES}
                                        ${LIBRARY_HEADER_FILES})

target_link_libraries(${PLEXY_CORE_LIBRARY} ${LIBRARY_LINK_TARGETS})

qt5_use_modules(${PLEXY_CORE_LIBRARY} Core Widgets)

generate_export_header( ${PLEXY_CORE_LIBRARY}
                                             BASE_NAME ${PLEXY_CORE_LIBRARY}
                                             EXPORT_MACRO_NAME CORE_DECL_EXPORT
                                             EXPORT_FILE_NAME  plexy_core_exports.h
                                             STATIC_DEFINE ${PLEXY_CORE_LIBRARY}_BUILT_AS_STATIC)

#target properties for OS X
if(APPLE)
    set_target_properties(${PLEXY_CORE_LIBRARY} PROPERTIES FRAMEWORK 1)
endif()

#target properties for MINGW
if(MINGW)
      set_TARGET_PROPERTIES(${PLEXY_CORE_LIBRARY} PROPERTIES
                                                            IMPORT_SUFFIX ".lib"
                                                            IMPORT_PREFIX ""
                                                            PREFIX "")
endif()

if(UNIX AND NOT APPLE)
  set_target_properties(${PLEXY_CORE_LIBRARY} PROPERTIES SOVERSION "1.0.0")
  install(TARGETS ${PLEXY_CORE_LIBRARY} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} NAMELINK_SKIP)
endif()

if(APPLE)
   install(TARGETS ${PLEXY_CORE_LIBRARY}
                                        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
                                        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
                                        FRAMEWORK DESTINATION plexydesk.app/Contents/Frameworks)
endif(APPLE)

if(WIN32)
  # installs to the same path as the binary
  install(TARGETS ${PLEXY_CORE_LIBRARY} DESTINATION ${CMAKE_INSTALL_PREFIX})
endif()
