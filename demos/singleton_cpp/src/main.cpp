#include "so_singleton_api.h"

#include <atomic>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <thread>
#include <vector>

namespace {

constexpr int kThreadCount = 8;
constexpr int kLoopPerThread = 10000;

std::string HexAddr(std::uintptr_t addr) {
  std::ostringstream oss;
  oss << "0x" << std::hex << addr;
  return oss.str();
}

class StaticSingleton final {
 public:
  static StaticSingleton& Instance() {
    static StaticSingleton instance;
    return instance;
  }

  int Increment() {
    return value_.fetch_add(1, std::memory_order_relaxed) + 1;
  }

  int Value() const {
    return value_.load(std::memory_order_relaxed);
  }

 private:
  StaticSingleton() = default;
  StaticSingleton(const StaticSingleton&) = delete;
  StaticSingleton& operator=(const StaticSingleton&) = delete;

  std::atomic<int> value_{0};
};

class HeapSingleton final {
 public:
  static HeapSingleton& Instance() {
    // 故意不释放，避免程序退出阶段的静态析构顺序问题。
    std::call_once(init_flag_, [] {
      instance_ = new HeapSingleton();
    });
    return *instance_;
  }

  int Increment() {
    return value_.fetch_add(1, std::memory_order_relaxed) + 1;
  }

  int Value() const {
    return value_.load(std::memory_order_relaxed);
  }

 private:
  HeapSingleton() = default;
  HeapSingleton(const HeapSingleton&) = delete;
  HeapSingleton& operator=(const HeapSingleton&) = delete;

  static std::once_flag init_flag_;
  static HeapSingleton* instance_;
  std::atomic<int> value_{0};
};

std::once_flag HeapSingleton::init_flag_;
HeapSingleton* HeapSingleton::instance_ = nullptr;

template <typename Func>
void RunInThreads(Func func) {
  std::vector<std::thread> threads;
  threads.reserve(kThreadCount);
  for (int i = 0; i < kThreadCount; ++i) {
    threads.emplace_back([&func]() {
      for (int j = 0; j < kLoopPerThread; ++j) {
        func();
      }
    });
  }
  for (auto& thread : threads) {
    thread.join();
  }
}

void PrintSnapshot(const ConsumerSnapshot& snapshot) {
  std::cout << "[" << snapshot.name << "]\n"
            << "  local singleton addr    : "
            << HexAddr(snapshot.local_singleton_addr) << '\n'
            << "  exported singleton addr : "
            << HexAddr(snapshot.exported_singleton_addr) << '\n'
            << "  exported value          : " << snapshot.exported_value << '\n';
}

void DemoInProcessSingletons() {
  RunInThreads([]() {
    StaticSingleton::Instance().Increment();
  });
  RunInThreads([]() {
    HeapSingleton::Instance().Increment();
  });

  const int expected = kThreadCount * kLoopPerThread;
  std::cout << "[static local singleton]\n"
            << "  instance addr : "
            << HexAddr(reinterpret_cast<std::uintptr_t>(&StaticSingleton::Instance()))
            << '\n'
            << "  value         : " << StaticSingleton::Instance().Value()
            << " (expected " << expected << ")\n\n";

  std::cout << "[heap singleton + call_once]\n"
            << "  instance addr : "
            << HexAddr(reinterpret_cast<std::uintptr_t>(&HeapSingleton::Instance()))
            << '\n'
            << "  value         : " << HeapSingleton::Instance().Value()
            << " (expected " << expected << ")\n";
}

void DemoSoSafeSingleton() {
  const ConsumerSnapshot one = consumer_one_snapshot();
  const ConsumerSnapshot two = consumer_two_snapshot();

  PrintSnapshot(one);
  PrintSnapshot(two);

  const bool local_isolated =
      one.local_singleton_addr != two.local_singleton_addr;
  const bool exported_shared =
      one.exported_singleton_addr == two.exported_singleton_addr;

  std::cout << "\n[check]\n"
            << "  local singleton isolated in each .so : "
            << (local_isolated ? "YES" : "NO") << '\n'
            << "  exported singleton shared by process : "
            << (exported_shared ? "YES" : "NO") << '\n';
}

}  // namespace

int main() {
  std::cout << "== 1) Thread-safe singleton in one process ==\n";
  DemoInProcessSingletons();

  std::cout << "\n== 2) If exporting .so, keep one singleton owner ==\n";
  DemoSoSafeSingleton();
  return 0;
}
