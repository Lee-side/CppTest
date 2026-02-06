cmake_minimum_required(VERSION 3.16)
project(dlopen_symbol_collision LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

add_library(demo_one SHARED
  src/lib_one.cpp
)
target_include_directories(demo_one PRIVATE src)
target_compile_options(demo_one PRIVATE -g -O0)

add_library(demo_two SHARED
  src/lib_two.cpp
)
target_include_directories(demo_two PRIVATE src)
target_compile_options(demo_two PRIVATE -g -O0)

add_executable(dlopen_symbol_demo
  src/main.cpp
)
target_include_directories(dlopen_symbol_demo PRIVATE src)
target_compile_options(dlopen_symbol_demo PRIVATE -g -O0)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_link_libraries(demo_one PRIVATE dl)
  target_link_libraries(demo_two PRIVATE dl)
  target_link_libraries(dlopen_symbol_demo PRIVATE dl)
endif()

target_compile_definitions(dlopen_symbol_demo PRIVATE
  DEMO_LIB_PREFIX="${CMAKE_SHARED_LIBRARY_PREFIX}"
  DEMO_LIB_SUFFIX="${CMAKE_SHARED_LIBRARY_SUFFIX}"
)

set_target_properties(demo_one demo_two dlopen_symbol_demo PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)
