cmake_minimum_required(VERSION 3.16)
project(core_dump_cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

add_executable(core_dump_demo
  src/main.cpp
  src/A.cpp
  src/B.cpp
)

target_compile_options(core_dump_demo PRIVATE -g -O0)
