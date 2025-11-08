#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <cmath>

#include "core/Vec3.h"

namespace rt {

// Escritura de imagen en formato PPM (P3) para simplicidad
class ImageWriterPPM {
 public:
  static bool write(const std::string& path, int width, int height, const std::vector<Vec3>& pixels, bool applyGamma=true) {
    std::ofstream f(path);
    if (!f.is_open()) return false;
    f << "P3\n" << width << " " << height << "\n255\n";
    for (int j = 0; j < height; ++j) {
      for (int i = 0; i < width; ++i) {
        const Vec3& c = pixels[j * width + i];
        Vec3 out = clamp01(c);
        if (applyGamma) {
          // gamma aproximado 2.2
          out.x = std::pow(out.x, 1.0/2.2);
          out.y = std::pow(out.y, 1.0/2.2);
          out.z = std::pow(out.z, 1.0/2.2);
        }
        int ir = static_cast<int>(255.999 * out.x);
        int ig = static_cast<int>(255.999 * out.y);
        int ib = static_cast<int>(255.999 * out.z);
        f << ir << ' ' << ig << ' ' << ib << '\n';
      }
    }
    return true;
  }
};

}


