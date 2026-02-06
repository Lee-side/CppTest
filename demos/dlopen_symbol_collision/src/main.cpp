#include "api.h"

#include <dlfcn.h>

#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

using SnapshotFn = DemoSnapshot (*)(int);
using SharedComputeFn = int (*)(int);
using SharedTouchFn = int (*)();

void* MustLoadSymbol(void* handle, const char* symbol_name) {
  dlerror();
  void* symbol = dlsym(handle, symbol_name);
  const char* err = dlerror();
  if (err != nullptr || symbol == nullptr) {
    std::cerr << "dlsym failed for symbol '" << symbol_name << "': "
              << (err ? err : "null") << '\n';
    std::exit(1);
  }
  return symbol;
}

void* MustLoadLibrary(const std::filesystem::path& lib_path) {
  dlerror();
  void* handle = dlopen(lib_path.c_str(), RTLD_NOW | RTLD_GLOBAL);
  const char* err = dlerror();
  if (err != nullptr || handle == nullptr) {
    std::cerr << "dlopen failed for " << lib_path << ": "
              << (err ? err : "null") << '\n';
    std::exit(1);
  }
  return handle;
}

std::string HexAddr(std::uintptr_t addr) {
  std::ostringstream oss;
  oss << "0x" << std::hex << addr;
  return oss.str();
}

void PrintSnapshot(const DemoSnapshot& snap) {
  std::cout << "[" << snap.lib_name << "]\n"
            << "  shared var  value=" << snap.shared_var_value
            << ", addr=" << HexAddr(snap.shared_var_addr) << '\n'
            << "  shared fn   ret=" << snap.shared_fn_result
            << ", addr=" << HexAddr(snap.shared_fn_addr) << '\n'
            << "  shared touch ret=" << snap.shared_touch_result
            << ", addr=" << HexAddr(snap.shared_touch_addr) << '\n'
            << "  default var value=" << snap.default_shared_var_value
            << ", addr=" << HexAddr(snap.default_shared_var_addr) << '\n'
            << "  default fn  ret=" << snap.default_shared_fn_result
            << ", addr=" << HexAddr(snap.default_shared_fn_addr) << '\n'
            << "  unique var  value=" << snap.unique_var_value
            << ", addr=" << HexAddr(snap.unique_var_addr) << '\n'
            << "  unique fn   ret=" << snap.unique_fn_result
            << ", addr=" << HexAddr(snap.unique_fn_addr) << '\n';
}

}  // namespace

int main(int argc, char** argv) {
  const std::string order = (argc >= 2) ? argv[1] : "12";
  if (order != "12" && order != "21") {
    std::cerr << "Usage: " << argv[0] << " [12|21]\n";
    return 1;
  }

  const std::filesystem::path exe_path =
      std::filesystem::canonical(std::filesystem::path(argv[0]));
  const std::filesystem::path build_dir = exe_path.parent_path();

  const std::string lib1_name =
      std::string(DEMO_LIB_PREFIX) + "demo_one" + DEMO_LIB_SUFFIX;
  const std::string lib2_name =
      std::string(DEMO_LIB_PREFIX) + "demo_two" + DEMO_LIB_SUFFIX;

  std::vector<std::string> load_seq =
      (order == "12") ? std::vector<std::string>{lib1_name, lib2_name}
                      : std::vector<std::string>{lib2_name, lib1_name};

  std::cout << "Load order: " << load_seq[0] << " -> " << load_seq[1] << "\n\n";

  void* handle_first = MustLoadLibrary(build_dir / load_seq[0]);
  void* handle_second = MustLoadLibrary(build_dir / load_seq[1]);

  auto* one_fn = reinterpret_cast<SnapshotFn>(
      MustLoadSymbol(handle_first, (order == "12") ? "demo_one_snapshot"
                                                    : "demo_two_snapshot"));
  auto* two_fn = reinterpret_cast<SnapshotFn>(
      MustLoadSymbol(handle_second, (order == "12") ? "demo_two_snapshot"
                                                     : "demo_one_snapshot"));

  const int x = 7;
  DemoSnapshot first = one_fn(x);
  DemoSnapshot second = two_fn(x);

  PrintSnapshot(first);
  std::cout << '\n';
  PrintSnapshot(second);
  std::cout << '\n';

  auto* default_shared_var = reinterpret_cast<int*>(
      MustLoadSymbol(RTLD_DEFAULT, "g_shared_value"));
  auto* default_shared_compute = reinterpret_cast<SharedComputeFn>(
      MustLoadSymbol(RTLD_DEFAULT, "shared_compute"));
  auto* default_shared_touch = reinterpret_cast<SharedTouchFn>(
      MustLoadSymbol(RTLD_DEFAULT, "shared_touch"));

  std::cout << "[RTLD_DEFAULT]\n"
            << "  g_shared_value=" << *default_shared_var
            << ", addr=" << HexAddr(reinterpret_cast<std::uintptr_t>(default_shared_var))
            << '\n'
            << "  shared_compute(" << x << ")=" << default_shared_compute(x)
            << ", addr="
            << HexAddr(reinterpret_cast<std::uintptr_t>(default_shared_compute))
            << '\n'
            << "  shared_touch()=" << default_shared_touch()
            << ", addr="
            << HexAddr(reinterpret_cast<std::uintptr_t>(default_shared_touch))
            << '\n';

  dlclose(handle_second);
  dlclose(handle_first);
  return 0;
}
