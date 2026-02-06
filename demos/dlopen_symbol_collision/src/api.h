#pragma once

#include <cstdint>

struct DemoSnapshot {
  const char* lib_name;

  int shared_var_value;
  std::uintptr_t shared_var_addr;

  int shared_fn_result;
  std::uintptr_t shared_fn_addr;

  int shared_touch_result;
  std::uintptr_t shared_touch_addr;

  int default_shared_var_value;
  std::uintptr_t default_shared_var_addr;

  int default_shared_fn_result;
  std::uintptr_t default_shared_fn_addr;

  int unique_var_value;
  std::uintptr_t unique_var_addr;

  int unique_fn_result;
  std::uintptr_t unique_fn_addr;
};

extern "C" {
DemoSnapshot demo_one_snapshot(int x);
DemoSnapshot demo_two_snapshot(int x);
}
