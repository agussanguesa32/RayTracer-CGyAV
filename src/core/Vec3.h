#pragma once

#include <cmath>
#include <algorithm>
#include <iostream>

// Vec3 simple para posiciones, direcciones y colores (RGB en [0,1])
// Comentarios en espanol y sin acentos, como pidio el usuario
namespace rt {

struct Vec3 {
  double x;
  double y;
  double z;

  Vec3() : x(0), y(0), z(0) {}
  Vec3(double x, double y, double z) : x(x), y(y), z(z) {}

  inline Vec3 operator+(const Vec3& v) const { return Vec3{x + v.x, y + v.y, z + v.z}; }
  inline Vec3 operator-(const Vec3& v) const { return Vec3{x - v.x, y - v.y, z - v.z}; }
  inline Vec3 operator-() const { return Vec3{-x, -y, -z}; }
  inline Vec3 operator*(const Vec3& v) const { return Vec3{x * v.x, y * v.y, z * v.z}; }
  inline Vec3 operator*(double s) const { return Vec3{x * s, y * s, z * s}; }
  inline Vec3 operator/(double s) const { return Vec3{x / s, y / s, z / s}; }

  inline Vec3& operator+=(const Vec3& v) { x += v.x; y += v.y; z += v.z; return *this; }
  inline Vec3& operator-=(const Vec3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
  inline Vec3& operator*=(double s) { x *= s; y *= s; z *= s; return *this; }
  inline Vec3& operator/=(double s) { x /= s; y /= s; z /= s; return *this; }

  inline double length() const { return std::sqrt(x*x + y*y + z*z); }
  inline double lengthSquared() const { return x*x + y*y + z*z; }
};

inline Vec3 operator*(double s, const Vec3& v) { return v * s; }

inline double dot(const Vec3& a, const Vec3& b) {
  return a.x*b.x + a.y*b.y + a.z*b.z;
}

inline Vec3 cross(const Vec3& a, const Vec3& b) {
  return Vec3{ a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x };
}

inline Vec3 normalize(const Vec3& v) {
  double len = v.length();
  if (len == 0) return v;
  return v / len;
}

inline Vec3 clamp01(const Vec3& c) {
  return Vec3{ std::clamp(c.x, 0.0, 1.0), std::clamp(c.y, 0.0, 1.0), std::clamp(c.z, 0.0, 1.0) };
}

// Reflexion de un vector incidente i alrededor de la normal n (n unitaria)
inline Vec3 reflect(const Vec3& i, const Vec3& n) {
  return i - 2.0 * dot(i, n) * n;
}

// refraccion segun Snell
// uv: direccion del rayo incidente NORMALIZADA (direccion de propagacion)
// n: normal unitaria que apunta hacia el medio de entrada (donde viene el rayo)
// etai_over_etat: ratio de indices de refraccion (ior_entrada / ior_salida)
// refracted: direccion refractada (salida)
// devuelve false si hay reflexion interna total (corroborar)
inline bool refract(const Vec3& uv, const Vec3& n, double etai_over_etat, Vec3& refracted) {
  double cos_theta = std::fmin(-dot(uv, n), 1.0);
  double sin_theta_sq = 1.0 - cos_theta * cos_theta;

  // verificamos reflexion interna total
  if (etai_over_etat * etai_over_etat * sin_theta_sq > 1.0) {
    return false;
  }

  Vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
  Vec3 r_out_parallel = -std::sqrt(std::fabs(1.0 - r_out_perp.lengthSquared())) * n;
  refracted = r_out_perp + r_out_parallel;
  return true;
}

inline double schlickFresnel(double cosTheta, double iorFrom, double iorTo) {
  double r0 = (iorFrom - iorTo) / (iorFrom + iorTo);
  r0 = r0 * r0;
  double oneMinusCos = 1.0 - cosTheta;
  return r0 + (1.0 - r0) * oneMinusCos*oneMinusCos*oneMinusCos*oneMinusCos*oneMinusCos;
}

}


