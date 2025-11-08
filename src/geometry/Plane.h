#pragma once

#include <memory>

#include "geometry/Hittable.h"

namespace rt {

// plano infinito definido por normal unitaria n y valor d tal que n·p + d = 0
class Plane : public Hittable {
 public:
  Plane() = default;
  Plane(const Vec3& n, double d, std::shared_ptr<Material> m)
    : normalUnit(normalize(n)), dval(d), mat(std::move(m)) {}

  bool hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const override {
    double denom = dot(normalUnit, r.direction);
    if (std::fabs(denom) < 1e-9) return false; // paralelo
    double t = -(dot(normalUnit, r.origin) + dval) / denom;
    if (t < tMin || t > tMax) return false;
    rec.t = t;
    rec.point = r.at(t);
    rec.setFaceNormal(r, normalUnit);
    rec.material = mat;
    return true;
  }

 private:
  Vec3 normalUnit{0,1,0};
  double dval{0};
  std::shared_ptr<Material> mat;
};

}


