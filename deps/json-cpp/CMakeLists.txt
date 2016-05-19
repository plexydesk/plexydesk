include(GNUInstallDirs)

################################
# set lib version here

set(GENERIC_LIB_VERSION "1.6.0")
set(GENERIC_LIB_SOVERSION "2")

################################
# Add common source 

project(jsoncpp_ck)
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
add_library(jsoncpp_ck jsoncpp.cpp)


if (NOT APPLE)
if (NOT WIN32)
install(TARGETS jsoncpp_ck
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()

if (WIN32)
install(TARGETS jsoncpp_ck
        RUNTIME DESTINATION .
        LIBRARY DESTINATION .
        ARCHIVE DESTINATION sdk)
endif()
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
