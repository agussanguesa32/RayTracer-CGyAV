#pragma once

#include <vector>
#include <iostream>

#include "core/Vec3.h"
#include "core/Ray.h"
#include "scene/Scene.h"
#include "renderer/Integrator.h"
#include "utils/Random.h"

namespace rt {

enum class RenderMode { Final, Normals };

class Renderer {
 public:
  Renderer(int w, int h, int spp, int maxDepth)
    : width(w), height(h), spp(spp), maxDepth(maxDepth) {}

  template <typename CameraT>
  std::vector<Vec3> render(const Scene& scene, const CameraT& camera, RenderMode mode) {
    std::vector<Vec3> pixels(width * height);
    Random rng;
    Integrator integrator;

    for (int j = height - 1; j >= 0; --j) {
      for (int i = 0; i < width; ++i) {
        Vec3 color{0,0,0};
        for (int s = 0; s < spp; ++s) {
          double u = (i + (spp > 1 ? rng.uniform01() : 0.5)) / (double)width;
          double v = (j + (spp > 1 ? rng.uniform01() : 0.5)) / (double)height;
          Ray r = camera.getRay(u, v);
          if (mode == RenderMode::Normals) {
            HitRecord rec;
            if (scene.hit(r, 1e-4, 1e9, rec)) {
              Vec3 n = rec.normal;
              color += 0.5 * (n + Vec3{1,1,1});
            } else {
              color += Vec3{0,0,0};
            }
          } else {
            color += integrator.trace(scene, r, maxDepth);
          }
        }
        color /= (double)spp;
        pixels[(height - 1 - j) * width + i] = color;
      }
      // progreso por fila
      int filasHechas = height - j;
      int percent = (int)std::round(100.0 * filasHechas / (double)height);
      std::cout << "\rprogreso: " << percent << "%" << std::flush;
    }
    std::cout << "\n";
    return pixels;
  }

  int width{800};
  int height{600};
  int spp{1};
  int maxDepth{6};
};

}


