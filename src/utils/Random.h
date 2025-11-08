#pragma once

#include <random>

namespace rt {

class Random {
 public:
  Random() : rng(std::random_device{}()), dist01(0.0, 1.0) {}

  inline double uniform01() { return dist01(rng); }

 private:
  std::mt19937_64 rng;
  std::uniform_real_distribution<double> dist01;
};

}


