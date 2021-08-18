#pragma once

#include "vector.h"

class Light {
 public:
  Light(const Vector3f& position, const float& intensity)
      : position_(position), intensity_(intensity) {}

 private:
  Vector3f position_;
  Vector3f intensity_;
}