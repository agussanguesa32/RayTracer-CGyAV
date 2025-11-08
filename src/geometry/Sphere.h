#pragma once

#include <memory>

#include "geometry/Hittable.h"

namespace rt {

class Sphere : public Hittable {
 public:
  Sphere() = default;
  Sphere(const Vec3& c, double r, std::shared_ptr<Material> m)
    : center(c), radius(r), mat(std::move(m)) {}

  bool hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const override {
    Vec3 oc = r.origin - center;
    double a = r.direction.lengthSquared();
    double half_b = dot(oc, r.direction);
    double c = oc.lengthSquared() - radius*radius;
    double discriminant = half_b*half_b - a*c;
    if (discriminant < 0) return false;
    double sqrtD = std::sqrt(discriminant);

    // tomar la raiz mas chica en el rango
    double root = (-half_b - sqrtD) / a;
    if (root < tMin || root > tMax) {
      root = (-half_b + sqrtD) / a;
      if (root < tMin || root > tMax) return false;
    }

    rec.t = root;
    rec.point = r.at(rec.t);
    Vec3 outward = (rec.point - center) / radius;
    rec.setFaceNormal(r, outward);
    rec.material = mat;
    return true;
  }

 private:
  Vec3 center{0,0,0};
  double radius{1.0};
  std::shared_ptr<Material> mat;
};

}


