cmake_minimum_required(VERSION 3.16)
project(cpp_std_lab LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

enable_testing()

function(add_cpp_std_lab_target std)
  set(target_name "cpp_std_lab_cpp${std}")

  add_executable(${target_name}
    src/main.cpp
  )

  set_target_properties(${target_name} PROPERTIES
    CXX_STANDARD ${std}
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
  )

  target_compile_definitions(${target_name} PRIVATE DEMO_STD=${std})
  target_compile_options(${target_name} PRIVATE -g -O0)
endfunction()

add_cpp_std_lab_target(17)
add_cpp_std_lab_target(20)
add_cpp_std_lab_target(23)

add_test(NAME cpp17_default
  COMMAND cpp_std_lab_cpp17 nth_element
)
set_tests_properties(cpp17_default PROPERTIES
  PASS_REGULAR_EXPRESSION "STD=17.*VALUE=5.*OK=1"
)

add_test(NAME cpp20_default
  COMMAND cpp_std_lab_cpp20 nth_element
)
set_tests_properties(cpp20_default PROPERTIES
  PASS_REGULAR_EXPRESSION "STD=20.*VALUE=5.*OK=1"
)

add_test(NAME cpp23_default
  COMMAND cpp_std_lab_cpp23 nth_element
)
set_tests_properties(cpp23_default PROPERTIES
  PASS_REGULAR_EXPRESSION "STD=23.*VALUE=5.*OK=1"
)

add_test(NAME cpp20_custom_dup
  COMMAND cpp_std_lab_cpp20 nth_element --nums 9,1,4,4,8 --k 3
)
set_tests_properties(cpp20_custom_dup PROPERTIES
  PASS_REGULAR_EXPRESSION "STD=20.*VALUE=4.*OK=1"
)

add_test(NAME cpp20_custom_neg
  COMMAND cpp_std_lab_cpp20 nth_element --nums -1,7,0,-3,2 --k 4
)
set_tests_properties(cpp20_custom_neg PROPERTIES
  PASS_REGULAR_EXPRESSION "STD=20.*VALUE=-1.*OK=1"
)

add_test(NAME cpp20_invalid_k
  COMMAND cpp_std_lab_cpp20 nth_element --nums 1,2 --k 0
)
set_tests_properties(cpp20_invalid_k PROPERTIES
  WILL_FAIL TRUE
)

add_test(NAME unknown_algo
  COMMAND cpp_std_lab_cpp20 not_exist_algo
)
set_tests_properties(unknown_algo PROPERTIES
  WILL_FAIL TRUE
)
