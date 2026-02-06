#pragma once

#include <cstdint>

#include "singleton_export.h"

struct ConsumerSnapshot {
  const char* name;
  std::uintptr_t local_singleton_addr;
  std::uintptr_t exported_singleton_addr;
  int exported_value;
};

extern "C" {

SINGLETON_DEMO_EXPORT int* so_singleton_instance();
SINGLETON_DEMO_EXPORT int so_singleton_next();

SINGLETON_DEMO_EXPORT ConsumerSnapshot consumer_one_snapshot();
SINGLETON_DEMO_EXPORT ConsumerSnapshot consumer_two_snapshot();

}  // extern "C"
