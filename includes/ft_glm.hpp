#pragma once

#include <array>
#include <glm/glm.hpp>

namespace ft_glm {

class vec2 {
public:
  float x, y;

  inline vec2(float x = 1.0f, float y = 1.0f) : x(x), y(y) {}
};

class vec3 {
public:
  float x, y, z;

  inline vec3(float x = 1.0f, float y = 1.0f, float z = 1.0f)
      : x(x), y(y), z(z) {}

  // vec3 = glm::vec3を可能にする
  inline vec3(const glm::vec3 &v) : x(v.x), y(v.y), z(v.z) {}

  inline vec3(const vec3 &v) : x(v.x), y(v.y), z(v.z) {}

  void zero() { x = y = z = 0.0f; }

  vec3 operator -() const { return vec3(-x, -y, -z); }

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

  void normalize() {
    // 方向ベクトルにしている
	float magSq = x * x + y * y + z * z;
	if (magSq > 0.0f) {
		float oneOverMag = 1.0f / std::sqrt(magSq);
		x *= oneOverMag;
		y *= oneOverMag;
		z *= oneOverMag;
	}
  }

  // dot product
  float operator*(const vec3 &v) const { return x * v.x + y * v.y + z * v.z; }

};

// non-member function
inline float magnitude(const vec3 &v) { return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z); }

inline vec3 cross(const vec3 &a, const vec3 &b) {
	return vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

inline vec3 normalize(const vec3 &v) {
  float magSq = v.x * v.x + v.y * v.y + v.z * v.z;
  if (magSq > 0.0f) {
	float oneOverMag = 1.0f / std::sqrt(magSq);
	return vec3(v.x * oneOverMag, v.y * oneOverMag, v.z * oneOverMag);
  }
  return vec3(0.0f, 0.0f, 0.0f);
}

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

Mat4 rotate(Mat4 const &m, float angle, vec3 const &v);
inline std::array<float, 4> vec4MulScalar(const std::array<float, 4> &v,
                                          float scalar) {
  return {v[0] * scalar, v[1] * scalar, v[2] * scalar, v[3] * scalar};
}

inline std::array<float, 4> vec4Add(const std::array<float, 4> &v,
                                    const std::array<float, 4> &v2) {
  return {v[0] + v2[0], v[1] + v2[1], v[2] + v2[2], v[3] + v2[3]};
}
} // namespace ft_glm
