#pragma once

#include "core/Vec3.h"

namespace rt {

// material base con parametros para Phong y propiedades de reflexion/refraccion
class Material {
 public:
  virtual ~Material() = default;

  Vec3 Ka{0.05, 0.05, 0.05}; // ambiente
  Vec3 Kd{0.8, 0.8, 0.8};    // difusa
  Vec3 Ks{0.0, 0.0, 0.0};    // especular
  double shininess{32.0};

  double reflectivity{0.0};  // coef de reflexion [0,1]
  double transparency{0.0};  // coef de transmision [0,1]
  double ior{1.0};           // indice de refraccion (vidrio 1.5)
  double fuzz{0.0};          // borrosidad metal
  Vec3 transmissionTint{1.0, 1.0, 1.0};
  Vec3 absorption{0.0, 0.0, 0.0};

  // emision propia (para simular luces visibles) y control de sombras
  Vec3 emissive{0.0, 0.0, 0.0};
  bool castsShadow{true};

  inline bool isReflective() const { return reflectivity > 0.0; }
  inline bool isRefractive() const { return transparency > 0.0; }
};

class Lambertian : public Material {
 public:
  Lambertian(const Vec3& color) {
    Kd = color;
    Ks = Vec3{0.0, 0.0, 0.0};
    shininess = 8.0;
    reflectivity = 0.0;
    transparency = 0.0;
    ior = 1.0;
  }
};

class Metal : public Material {
 public:
  Metal(const Vec3& color, double fuzziness = 0.0, double kr = 1.0) {
    Kd = Vec3{0.0, 0.0, 0.0};
    Ks = color;
    shininess = 128.0;
    reflectivity = kr;
    transparency = 0.0;
    ior = 1.0;
    fuzz = fuzziness;
  }
};

class Dielectric : public Material {
 public:
  Dielectric(double indexOfRefraction, const Vec3& tint = Vec3{1.0, 1.0, 1.0}) {
    Ka = Vec3{0.0, 0.0, 0.0};
    Kd = Vec3{0.0, 0.0, 0.0};
    Ks = Vec3{1.0, 1.0, 1.0}; 
    shininess = 200.0;        
    reflectivity = 1.0; 
    transparency = 1.0;
    ior = indexOfRefraction;
    castsShadow = false;
    transmissionTint = tint;  
    absorption = Vec3{0.0, 0.0, 0.0}; 
  }
};

}


