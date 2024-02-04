#pragma once

#include <array>

class vec3 {
public:
  float x, y, z;

  inline vec3(float x = 1.0f, float y = 1.0f, float z = 1.0f)
      : x(x), y(y), z(z) {}

  inline vec3 operator+(const vec3 &v) const {
    return vec3(x + v.x, y + v.y, z + v.z);
  }

  inline vec3 operator-(const vec3 &v) const {
    return vec3(x - v.x, y - v.y, z - v.z);
  }

  inline vec3 operator*(float scalar) const {
    return vec3(x * scalar, y * scalar, z * scalar);
  }

  inline float dot(const vec3 &v) const { return x * v.x + y * v.y + z * v.z; }

  inline vec3 cross(const vec3 &v) const {
    return vec3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
  }

  inline float magnitude() const { return std::sqrt(x * x + y * y + z * z); }

  inline vec3 normalize() const {
    float m = magnitude();
    return vec3(x / m, y / m, z / m);
  }
};

class Mat4 {
private:
  std::array<std::array<float, 4>, 4> mat;

public:
  Mat4(float defaulValue);

  inline std::array<float, 4> &operator[](int index) { return mat[index]; }

  inline const std::array<float, 4> &operator[](int index) const {
    return mat[index];
  }

  Mat4 operator+(const Mat4 &other) const;
  Mat4 operator-(const Mat4 &other) const;
  Mat4 operator*(float scalar) const;
  Mat4 operator*(const Mat4 &other) const;

  void print() const;
};
