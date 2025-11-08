#pragma once

#include "core/Vec3.h"

namespace rt {

struct Ray {
  Vec3 origin;
  Vec3 direction;

  Ray() = default;
  Ray(const Vec3& o, const Vec3& d) : origin(o), direction(d) {}

  inline Vec3 at(double t) const { return origin + direction * t; }
};

}


