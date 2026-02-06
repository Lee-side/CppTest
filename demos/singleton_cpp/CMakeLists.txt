cmake_minimum_required(VERSION 3.16)
project(singleton_cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

add_library(singleton_owner SHARED
  src/so_singleton_owner.cpp
)
target_include_directories(singleton_owner PUBLIC src)
target_compile_options(singleton_owner PRIVATE -g -O0)
set_target_properties(singleton_owner PROPERTIES
  CXX_VISIBILITY_PRESET hidden
  VISIBILITY_INLINES_HIDDEN YES
)

add_library(singleton_consumer_one SHARED
  src/consumer_one.cpp
)
target_include_directories(singleton_consumer_one PRIVATE src)
target_link_libraries(singleton_consumer_one PRIVATE singleton_owner)
target_compile_options(singleton_consumer_one PRIVATE -g -O0)
set_target_properties(singleton_consumer_one PROPERTIES
  CXX_VISIBILITY_PRESET hidden
  VISIBILITY_INLINES_HIDDEN YES
)

add_library(singleton_consumer_two SHARED
  src/consumer_two.cpp
)
target_include_directories(singleton_consumer_two PRIVATE src)
target_link_libraries(singleton_consumer_two PRIVATE singleton_owner)
target_compile_options(singleton_consumer_two PRIVATE -g -O0)
set_target_properties(singleton_consumer_two PROPERTIES
  CXX_VISIBILITY_PRESET hidden
  VISIBILITY_INLINES_HIDDEN YES
)

add_executable(singleton_demo
  src/main.cpp
)
target_include_directories(singleton_demo PRIVATE src)
target_link_libraries(singleton_demo PRIVATE
  singleton_owner
  singleton_consumer_one
  singleton_consumer_two
)
target_compile_options(singleton_demo PRIVATE -g -O0)

set_target_properties(
  singleton_owner
  singleton_consumer_one
  singleton_consumer_two
  singleton_demo
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)
