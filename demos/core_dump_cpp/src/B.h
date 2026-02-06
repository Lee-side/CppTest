#pragma once

#include "A.h"

class B final : public A {
public:
  B();
  ~B() override;

private:
  int* ptr_;
};
