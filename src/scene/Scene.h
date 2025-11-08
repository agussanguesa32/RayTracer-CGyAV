#pragma once

#include <vector>
#include <memory>

#include "geometry/Hittable.h"
#include "lights/PointLight.h"

namespace rt {

class Scene {
 public:
  void addObject(const std::shared_ptr<Hittable>& obj) { objects.push_back(obj); }
  void addLight(const PointLight& l) { lights.push_back(l); }

  bool hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const {
    HitRecord temp;
    bool hitAnything = false;
    double closest = tMax;
    for (const auto& obj : objects) {
      if (obj->hit(r, tMin, closest, temp)) {
        hitAnything = true;
        closest = temp.t;
        rec = temp;
      }
    }
    return hitAnything;
  }

  // test de oclusion para rayos de sombra: ignora objetos que no proyectan sombra
  bool isOccluded(const Ray& r, double tMin, double tMax) const {
    HitRecord temp;
    for (const auto& obj : objects) {
      if (obj->hit(r, tMin, tMax, temp)) {
        if (temp.material && temp.material->castsShadow) return true;
      }
    }
    return false;
  }

  std::vector<std::shared_ptr<Hittable>> objects;
  std::vector<PointLight> lights;
  Vec3 background{0.7, 0.8, 1.0}; // cielo
};

}


