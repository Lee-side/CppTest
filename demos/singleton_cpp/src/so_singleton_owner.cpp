#include "so_singleton_api.h"

#include <mutex>

namespace {

class SoSingleton final {
 public:
  static SoSingleton& Instance() {
    static SoSingleton instance;
    return instance;
  }

  int Next() {
    std::lock_guard<std::mutex> lock(mu_);
    return ++value_;
  }

  int* ValuePtr() {
    return &value_;
  }

 private:
  SoSingleton() = default;
  ~SoSingleton() = default;
  SoSingleton(const SoSingleton&) = delete;
  SoSingleton& operator=(const SoSingleton&) = delete;

  int value_ = 0;
  std::mutex mu_;
};

}  // namespace

extern "C" SINGLETON_DEMO_EXPORT int* so_singleton_instance() {
  return SoSingleton::Instance().ValuePtr();
}

extern "C" SINGLETON_DEMO_EXPORT int so_singleton_next() {
  return SoSingleton::Instance().Next();
}
