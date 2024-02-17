#pragma once

#include <array>
#include <glm/glm.hpp>

namespace ft_glm {

class vec3 {
public:
  float x, y, z;

  inline vec3(float x = 1.0f, float y = 1.0f, float z = 1.0f)
      : x(x), y(y), z(z) {}

  // vec3 = glm::vec3を可能にする
  inline vec3(const glm::vec3 &v) : x(v.x), y(v.y), z(v.z) {}

  inline vec3 operator+(const vec3 &v) const {
    return vec3(x + v.x, y + v.y, z + v.z);
  }

  inline vec3 operator-(const vec3 &v) const {
    return vec3(x - v.x, y - v.y, z - v.z);
  }

  inline vec3 operator*(float scalar) const {
    return vec3(x * scalar, y * scalar, z * scalar);
  }

  inline vec3 operator+=(const vec3 &v) {
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
  }

  inline vec3 operator-=(const vec3 &v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
  }

  inline vec3 operator*=(const vec3 &v) {
    x *= v.x;
    y *= v.y;
    z *= v.z;
    return *this;
  }

  inline vec3 operator/=(const vec3 &v) {
    x /= v.x;
    y /= v.y;
    z /= v.z;
    return *this;
  }

  inline float dot(const vec3 &v) const { return x * v.x + y * v.y + z * v.z; }

  inline vec3 cross(const vec3 &v) const {
    return vec3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
  }

  inline float magnitude() const { return std::sqrt(x * x + y * y + z * z); }

  inline vec3 normalize() const {
	// 方向ベクトルにしている
    float m = magnitude();
    return vec3(x / m, y / m, z / m);
  }
};

class Mat4 {
private:
  std::array<std::array<float, 4>, 4> mat;

public:
  Mat4(float defaulValue = 1.0f);

  // Mat4 = glm::mat4を可能にする
  inline Mat4(const glm::mat4 &m) {
    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < 4; ++j) {
        mat[i][j] = m[i][j];
      }
    }
  }

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

Mat4 perspective(float fov, float aspect, float zNear, float zFar);
Mat4 lookAt(const vec3 &eye, const vec3 &center, const vec3 &up);
inline double radians(double degrees) { return degrees * (M_PI / 180.0); }
} // namespace ft_glm
