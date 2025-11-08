#pragma once

#include <string>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <unistd.h>

#include "core/Vec3.h"
#include "utils/ImageWriterPPM.h"

namespace rt {

class ImageWriterAuto {
 public:
  static bool write(const std::string& path, int width, int height, const std::vector<Vec3>& pixels, bool applyGamma=true) {
    if (hasExtension(path, ".ppm")) {
      return ImageWriterPPM::write(path, width, height, pixels, applyGamma);
    }
    if (hasExtension(path, ".png")) {
      return writePNG(path, width, height, pixels, applyGamma);
    }
    // por defecto, usa PPM
    return ImageWriterPPM::write(path + ".ppm", width, height, pixels, applyGamma);
  }

 private:
  static bool hasExtension(const std::string& s, const std::string& ext) {
    if (s.size() < ext.size()) return false;
    std::string tail = s.substr(s.size() - ext.size());
    for (auto& c : tail) c = (char)tolower(c);
    std::string extLow = ext;
    for (auto& c : extLow) c = (char)tolower(c);
    return tail == extLow;
  }

  static bool commandExists(const char* cmd) {
    std::ostringstream oss;
    oss << "command -v " << cmd << " > /dev/null 2>&1";
    int rc = std::system(oss.str().c_str());
    return rc == 0;
  }

  static bool writePNG(const std::string& path, int width, int height, const std::vector<Vec3>& pixels, bool applyGamma) {
    // escribir ppm temporal y convertir con pnmtopng o convert si existen
    std::ostringstream tmp;
    tmp << "/tmp/rt_tmp_" << getpid() << ".ppm";
    std::string tmpPath = tmp.str();
    if (!ImageWriterPPM::write(tmpPath, width, height, pixels, applyGamma)) return false;

    bool ok = false;
    if (commandExists("pnmtopng")) {
      std::ostringstream cmd;
      cmd << "pnmtopng \"" << tmpPath << "\" > \"" << path << "\"";
      ok = (std::system(cmd.str().c_str()) == 0);
    } else if (commandExists("convert")) {
      std::ostringstream cmd;
      cmd << "convert \"" << tmpPath << "\" \"" << path << "\"";
      ok = (std::system(cmd.str().c_str()) == 0);
    }

    // limpiar temporal
    std::ostringstream rm;
    rm << "rm -f \"" << tmpPath << "\"";
    std::system(rm.str().c_str());
    return ok;
  }
};

}


