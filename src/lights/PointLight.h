#pragma once

#include "core/Vec3.h"

namespace rt {

struct PointLight {
  Vec3 position{0,0,0};
  Vec3 color{1,1,1};
  double intensity{1.0};
};

}


