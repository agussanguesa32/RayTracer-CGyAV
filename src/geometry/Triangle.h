#pragma once

#include <memory>

#include "geometry/Hittable.h"

namespace rt {

class Triangle : public Hittable {
 public:
  Triangle() = default;
  Triangle(const Vec3& a, const Vec3& b, const Vec3& c, std::shared_ptr<Material> m)
    : v0(a), v1(b), v2(c), mat(std::move(m)) {
    normalFace = normalize(cross(v1 - v0, v2 - v0));
  }

  bool hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const override {
    // Moller Trumbore
    const double EPS = 1e-9;
    Vec3 edge1 = v1 - v0;
    Vec3 edge2 = v2 - v0;
    Vec3 pvec = cross(r.direction, edge2);
    double det = dot(edge1, pvec);
    if (std::fabs(det) < EPS) return false;
    double invDet = 1.0 / det;

    Vec3 tvec = r.origin - v0;
    double u = dot(tvec, pvec) * invDet;
    if (u < 0.0 || u > 1.0) return false;

    Vec3 qvec = cross(tvec, edge1);
    double v = dot(r.direction, qvec) * invDet;
    if (v < 0.0 || u + v > 1.0) return false;

    double t = dot(edge2, qvec) * invDet;
    if (t < tMin || t > tMax) return false;

    rec.t = t;
    rec.point = r.at(t);
    // normal plana de cara
    rec.setFaceNormal(r, normalFace);
    rec.material = mat;
    return true;
  }

 private:
  Vec3 v0, v1, v2;
  Vec3 normalFace;
  std::shared_ptr<Material> mat;
};

}


