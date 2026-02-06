#include "A.h"

#include <iostream>

A::A() {
  std::cout << "[A] ctor" << std::endl;
}

A::~A() {
  std::cout << "[A] dtor" << std::endl;
}
