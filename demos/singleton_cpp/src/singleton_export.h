#pragma once

#if defined(_WIN32)
#define SINGLETON_DEMO_EXPORT __declspec(dllexport)
#else
#define SINGLETON_DEMO_EXPORT __attribute__((visibility("default")))
#endif
