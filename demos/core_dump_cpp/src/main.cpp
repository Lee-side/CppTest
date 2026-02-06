#include "A.h"
#include "B.h"

#include <iostream>

int main() {
  std::cout << "Create B as A*, then delete it to trigger virtual destructor chain." << std::endl;
  A* obj = new B();
  delete obj;
  return 0;
}
