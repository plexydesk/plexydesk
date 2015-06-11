set(project_name jeff_build_training_data)
set(source
   main.cpp
   )

PROJECT(${project_name})

INCLUDE (${CMAKE_SOURCE_DIR}/deps/mitie/mitielib/cmake)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/deps/tinyxml2)

ADD_EXECUTABLE(${project_name} ${source})
TARGET_LINK_LIBRARIES(${project_name} mitie tinyxml2)

