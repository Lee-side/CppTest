#include "api.h"

#include <dlfcn.h>

extern "C" {

int g_shared_value = 111;
int g_shared_counter = 1000;

int shared_compute(int x) {
  return g_shared_value + x + 1;
}

int shared_touch() {
  return ++g_shared_counter;
}

int g_unique_one = 10001;

int unique_compute_one(int x) {
  return g_unique_one - x;
}

DemoSnapshot demo_one_snapshot(int x) {
  DemoSnapshot snap{};
  snap.lib_name = "demo_one";

  snap.shared_var_value = g_shared_value;
  snap.shared_var_addr = reinterpret_cast<std::uintptr_t>(&g_shared_value);

  snap.shared_fn_result = shared_compute(x);
  snap.shared_fn_addr = reinterpret_cast<std::uintptr_t>(&shared_compute);

  snap.shared_touch_result = shared_touch();
  snap.shared_touch_addr = reinterpret_cast<std::uintptr_t>(&shared_touch);

  auto* default_shared_var = reinterpret_cast<int*>(
      dlsym(RTLD_DEFAULT, "g_shared_value"));
  auto* default_shared_fn = reinterpret_cast<int (*)(int)>(
      dlsym(RTLD_DEFAULT, "shared_compute"));

  snap.default_shared_var_value =
      (default_shared_var != nullptr) ? *default_shared_var : -1;
  snap.default_shared_var_addr =
      reinterpret_cast<std::uintptr_t>(default_shared_var);
  snap.default_shared_fn_result =
      (default_shared_fn != nullptr) ? default_shared_fn(x) : -1;
  snap.default_shared_fn_addr =
      reinterpret_cast<std::uintptr_t>(default_shared_fn);

  snap.unique_var_value = g_unique_one;
  snap.unique_var_addr = reinterpret_cast<std::uintptr_t>(&g_unique_one);

  snap.unique_fn_result = unique_compute_one(x);
  snap.unique_fn_addr = reinterpret_cast<std::uintptr_t>(&unique_compute_one);

  return snap;
}

}  // extern "C"
