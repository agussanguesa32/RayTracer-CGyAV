#pragma once

#include "core/Vec3.h"
#include "core/Ray.h"

#include <cmath>

namespace rt {

class Camera {
 public:
  Camera(const Vec3& lookFrom, const Vec3& lookAt, const Vec3& vup,
         double vfovDeg, double aspect) {
    origin = lookFrom;
    double theta = vfovDeg * M_PI / 180.0;
    double h = std::tan(theta / 2.0);
    double viewportHeight = 2.0 * h;
    double viewportWidth = aspect * viewportHeight;

    // base ortonormal
    w = normalize(lookFrom - lookAt);
    u = normalize(cross(vup, w));
    v = cross(w, u);

    horizontal = viewportWidth * u;
    vertical = viewportHeight * v;
    lowerLeftCorner = origin - horizontal * 0.5 - vertical * 0.5 - w;
  }

  Ray getRay(double s, double t) const {
    Vec3 dir = lowerLeftCorner + s * horizontal + t * vertical - origin;
    return Ray(origin, normalize(dir));
  }

 private:
  Vec3 origin;
  Vec3 lowerLeftCorner;
  Vec3 horizontal;
  Vec3 vertical;
  Vec3 u, v, w;
};

}


