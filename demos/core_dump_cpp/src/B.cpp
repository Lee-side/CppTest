#include "B.h"

#include <iostream>

B::B() : ptr_(nullptr) {
  std::cout << "[B] ctor, ptr_ = nullptr" << std::endl;
}

B::~B() {
  std::cout << "[B] dtor: demo starts" << std::endl;
  std::cout << "[B] try release nullptr (delete nullptr is safe in C++)" << std::endl;
  delete ptr_;

  std::cout << "[B] force crash for core dump by writing nullptr" << std::endl;
  *ptr_ = 42;  // NOLINT intentionally crash for demo.
}
