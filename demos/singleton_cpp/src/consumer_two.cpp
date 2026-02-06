#include "so_singleton_api.h"

namespace {

class LocalSingletonTwo final {
 public:
  static LocalSingletonTwo& Instance() {
    static LocalSingletonTwo instance;
    return instance;
  }

 private:
  LocalSingletonTwo() = default;
};

}  // namespace

extern "C" SINGLETON_DEMO_EXPORT ConsumerSnapshot consumer_two_snapshot() {
  ConsumerSnapshot snapshot{};
  snapshot.name = "consumer_two";
  snapshot.local_singleton_addr =
      reinterpret_cast<std::uintptr_t>(&LocalSingletonTwo::Instance());

  int* exported_instance = so_singleton_instance();
  snapshot.exported_singleton_addr =
      reinterpret_cast<std::uintptr_t>(exported_instance);
  snapshot.exported_value = so_singleton_next();
  return snapshot;
}
