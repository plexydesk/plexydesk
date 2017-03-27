CMAKE_MINIMUM_REQUIRED(VERSION 3.0.1)
include (GenerateExportHeader)
CMAKE_POLICY(SET CMP0043 NEW)


project(tinyxml2)
include(GNUInstallDirs)

set(GENERIC_LIB_VERSION "3.0.0")
set(GENERIC_LIB_SOVERSION "3")


################################
# Add common source 

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/.")

if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif(MSVC)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

################################
# Add targets
option(BUILD_SHARED_LIBS "build shared or static libraries" ON)
add_library(tinyxml2 SHARED tinyxml2.cpp tinyxml2.h)

if (NOT APPLE)
set_target_properties(tinyxml2 PROPERTIES
        COMPILE_DEFINITIONS "TINYXML2_EXPORT"
	VERSION "${GENERIC_LIB_VERSION}"
	SOVERSION "${GENERIC_LIB_SOVERSION}")
endif()

if (NOT APPLE)
if (NOT WIN32)
install(TARGETS tinyxml2
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()

if (WIN32)
install(TARGETS tinyxml2
                        RUNTIME DESTINATION .
                        LIBRARY DESTINATION .
                        ARCHIVE DESTINATION sdk)
endif()
endif()


if (APPLE)
    INSTALL(TARGETS tinyxml2 LIBRARY DESTINATION plexydesk.app/Contents/Frameworks
                    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
                    FRAMEWORK DESTINATION plexydesk.app/Contents/Frameworks)
endif(APPLE)
