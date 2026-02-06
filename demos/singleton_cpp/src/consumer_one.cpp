#include "so_singleton_api.h"

namespace {

class LocalSingletonOne final {
 public:
  static LocalSingletonOne& Instance() {
    static LocalSingletonOne instance;
    return instance;
  }

 private:
  LocalSingletonOne() = default;
};

}  // namespace

extern "C" SINGLETON_DEMO_EXPORT ConsumerSnapshot consumer_one_snapshot() {
  ConsumerSnapshot snapshot{};
  snapshot.name = "consumer_one";
  snapshot.local_singleton_addr =
      reinterpret_cast<std::uintptr_t>(&LocalSingletonOne::Instance());

  int* exported_instance = so_singleton_instance();
  snapshot.exported_singleton_addr =
      reinterpret_cast<std::uintptr_t>(exported_instance);
  snapshot.exported_value = so_singleton_next();
  return snapshot;
}
