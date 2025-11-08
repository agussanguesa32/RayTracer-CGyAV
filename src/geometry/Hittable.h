#pragma once

#include <memory>

#include "core/Vec3.h"
#include "core/Ray.h"

namespace rt {

struct Material;

struct HitRecord {
  Vec3 point;
  Vec3 normal;
  double t = 0.0;
  bool frontFace = true;
  std::shared_ptr<Material> material;

  inline void setFaceNormal(const Ray& r, const Vec3& outwardNormal) {
    frontFace = dot(r.direction, outwardNormal) < 0.0;
    normal = frontFace ? outwardNormal : outwardNormal * -1.0;
  }
};

class Hittable {
 public:
  virtual ~Hittable() = default;
  virtual bool hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const = 0;
};

}


