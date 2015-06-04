SET (project_name query_jeff)

PROJECT(${project_name})

SET (source main.cpp)

INCLUDE (${CMAKE_SOURCE_DIR}/deps/mitie/mitielib/cmake)

ADD_EXECUTABLE(${project_name} ${source})
TARGET_LINK_LIBRARIES(${project_name} mitie)
