#pragma once

#include <algorithm>

#include "scene/Scene.h"

namespace rt {

class Integrator {
 public:
  // Traza un rayo con recursion limitada por maxDepth
  Vec3 trace(const Scene& scene, const Ray& ray, int depth) const {
    if (depth <= 0) return scene.background;

    HitRecord rec;
    if (!scene.hit(ray, 1e-4, 1e9, rec)) {
      return scene.background;
    }

    // componente ambiente y emision propia del material
    Vec3 color = rec.material->Ka + rec.material->emissive;

    // iluminacion directa Phong con sombras duras
    if (!rec.material->isRefractive()) {
      for (const auto& light : scene.lights) {
        Vec3 toLight = light.position - rec.point;
        double distLight = toLight.length();
        Vec3 sdir = toLight / distLight;

        // rayo de sombra
        Ray shadowRay(rec.point + rec.normal * 1e-4, sdir);
        HitRecord tmp;
        bool occluded = scene.isOccluded(shadowRay, 1e-4, distLight - 1e-4);
        if (occluded) continue;

        double ndotl = std::max(0.0, dot(rec.normal, sdir));
        // Atenuacion simple por distancia (suave)
        double fatt = 1.0 / (1.0 + 0.12 * distLight * distLight);
        Vec3 diffuse = rec.material->Kd * (light.color * (light.intensity * fatt * ndotl));

        Vec3 vdir = normalize(-ray.direction);
        Vec3 rdir = reflect(-sdir, rec.normal);
        double rdotv = std::max(0.0, dot(rdir, vdir));
        Vec3 specular = rec.material->Ks * (light.color * std::pow(rdotv, rec.material->shininess) * light.intensity * fatt);

        color += diffuse + specular;
      }
    }

    // reflexion y refraccion recursivas
    if (rec.material->isReflective() || rec.material->isRefractive()) {
      Vec3 reflColor{0,0,0};
      Vec3 refrColor{0,0,0};

      // REFLEXION
      if (rec.material->isReflective()) {
        Vec3 reflected = reflect(ray.direction, rec.normal);
        reflColor = trace(scene, Ray(rec.point + rec.normal * 1e-4, reflected), depth - 1);
      }

      // REFRACCION
      if (rec.material->isRefractive()) {
        double refraction_ratio = rec.frontFace ? (1.0 / rec.material->ior) : rec.material->ior;
        Vec3 unit_direction = normalize(ray.direction);
        Vec3 refracted;
        bool can_refract = refract(unit_direction, rec.normal, refraction_ratio, refracted);

        if (can_refract) {
          Vec3 origin = rec.point - rec.normal * 1e-4;
          double distInside = 0.0;
          if (rec.frontFace) {
            HitRecord exitRec;
            if (scene.hit(Ray(origin, refracted), 1e-4, 1e9, exitRec)) {
              distInside = exitRec.t;
            }
          }
          Vec3 att{
            std::exp(-rec.material->absorption.x * distInside),
            std::exp(-rec.material->absorption.y * distInside),
            std::exp(-rec.material->absorption.z * distInside)
          };
          refrColor = trace(scene, Ray(origin, refracted), depth - 1);
          refrColor = refrColor * att * rec.material->transmissionTint;
        } else {
          // Reflexion interna total
          Vec3 reflected = reflect(unit_direction, rec.normal);
          refrColor = trace(scene, Ray(rec.point + rec.normal * 1e-4, reflected), depth - 1);
        }

        // Mezcla fisicamente plausible por Fresnel (Schlick)
        double cosTheta = std::fmin(-dot(unit_direction, rec.normal), 1.0);
        double iorFrom = rec.frontFace ? 1.0 : rec.material->ior;
        double iorTo   = rec.frontFace ? rec.material->ior : 1.0;
        double kr = schlickFresnel(cosTheta, iorFrom, iorTo);

        // Si no calculamos refleccion antes (por material), la tomamos como 0
        color += kr * reflColor + (1.0 - kr) * refrColor;
      } else {
        // Material puramente reflectivo (metal)
        color += rec.material->reflectivity * reflColor;
      }
    }

    return color;
  }
};

}


