CMAKE_MINIMUM_REQUIRED(VERSION 3.0.1)
include (GenerateExportHeader)
set(CMAKE_AUTOMOC ON)
CMAKE_POLICY(SET CMP0043 NEW)

#project(json-cpp-ck)
include(GNUInstallDirs)

################################
# set lib version here

set(GENERIC_LIB_VERSION "1.6.0")
set(GENERIC_LIB_SOVERSION "2")

################################
# Add common source 

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/.")

################################
# Add definitions

if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif(MSVC)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

################################
# Add targets
option(BUILD_SHARED_LIBS "build shared or static libraries" ON)
add_library(jsoncpp_ck STATIC jsoncpp.cpp)
set_target_properties(jsoncpp_ck PROPERTIES
        COMPILE_DEFINITIONS "JSONCPP_EXPORT"
        VERSION "${GENERIC_LIB_VERSION}"
        SOVERSION "${GENERIC_LIB_SOVERSION}")


if (NOT APPLE)
install(TARGETS jsoncpp_ck
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()

if (APPLE)
  set (CMAKE_MACOSX_RPATH 1)
  SET_TARGET_PROPERTIES(jsoncpp_ck PROPERTIES FRAMEWORK 1)
  SET_TARGET_PROPERTIES(jsoncpp_ck PROPERTIES SKIP_BUILD_RPATH 0)

 	INSTALL(TARGETS   jsoncpp_ck 
                    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/
                    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
                    FRAMEWORK DESTINATION plexydesk.app/Contents/Frameworks)
endif()
#install(FILES jsoncpp.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

foreach(p LIB INCLUDE)
	set(var CMAKE_INSTALL_${p}DIR)
	if(NOT IS_ABSOLUTE "${${var}}")
		set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
	endif()
endforeach()

#configure_file(jsoncpp.pc.in jsoncpp.pc @ONLY)
#install(FILES ${CMAKE_CURRENT_BINARY_DIR}/jsoncpp.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

#add_test(xmltest ${SAMPLE_NAME} COMMAND $<TARGET_FILE:${SAMPLE_NAME}>)
