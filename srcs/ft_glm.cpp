#include <iostream>

#include "ft_glm.hpp"

namespace ft_glm {

Mat4::Mat4(float defaulValue) {
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      mat[i][j] = (i == j) ? defaulValue : 0.0f;
    }
  }
}

Mat4 Mat4::operator*(const Mat4 &other) const {
  Mat4 result(0.0f);
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      for (int k = 0; k < 4; ++k) {
        result.mat[j][i] += this->mat[k][i] * other.mat[j][k];
      }
    }
  }
  return result;
}

void Mat4::print() const {
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      std::cout << mat[i][j] << " ";
    }
    std::cout << std::endl;
  }
}

Mat4 perspective(float fov, float aspect, float zNear, float zFar) {
  const float tanHalfFovy = tan(fov / 2.0f);

  Mat4 result(0.0f);
  result[0][0] = 1.0f / (aspect * tanHalfFovy);
  result[1][1] = 1.0f / (tanHalfFovy);
  result[2][2] = -(zFar + zNear) / (zFar - zNear);
  result[3][2] = -(2 * zFar * zNear) / (zFar - zNear);

  result[2][3] = -1.0f;

  return result;
}

Mat4 lookAt(const vec3 &eye, const vec3 &center, const vec3 &up) {
  vec3 f = center - eye; // forward vector
  vec3 s = cross(f, up); // side vector
  vec3 u = cross(s, f);  // up vector

  f.normalize();
  s.normalize();
  u.normalize();

  Mat4 result(1.0f);
  result[0][0] = s.x;
  result[1][0] = s.y;
  result[2][0] = s.z;

  result[0][1] = u.x;
  result[1][1] = u.y;
  result[2][1] = u.z;

  result[0][2] = -f.x;
  result[1][2] = -f.y;
  result[2][2] = -f.z;

  // 内積で移動量を計算
  result[3][0] = -s * eye;
  result[3][1] = -u * eye;
  result[3][2] = f * eye;

  return result;
}

Mat4 translate(Mat4 const &m, vec3 const &v) {
  Mat4 Result = m;
  Result[3] = m[0] * v.x + m[1] * v.y + m[2] * v.z + m[3];
  return Result;
}

Mat4 rotate(Mat4 const &m, float angle, vec3 const &v) {
  // ロドリゲスの回転公式
  float const a = angle;
  float const c = cos(a);
  float const s = sin(a);

  vec3 axis(normalize(v));
  vec3 temp(axis * (1 - c));

  Mat4 Rotate;
  Rotate[0][0] = axis.x * temp.x + c;
  Rotate[0][1] = axis.x * temp.y + axis.z * s;
  Rotate[0][2] = axis.x * temp.z - axis.y * s;

  Rotate[1][0] = axis.x * temp.y - axis.z * s;
  Rotate[1][1] = axis.y * temp.y + c;
  Rotate[1][2] = axis.y * temp.z + axis.x * s;

  Rotate[2][0] = axis.x * temp.z + axis.y * s;
  Rotate[2][1] = axis.y * temp.z - axis.x * s;
  Rotate[2][2] = axis.z * temp.z + c;

  Mat4 Result;
  Result[0] = m[0] * Rotate[0][0] + m[1] * Rotate[0][1] + m[2] * Rotate[0][2];
  Result[1] = m[0] * Rotate[1][0] + m[1] * Rotate[1][1] + m[2] * Rotate[1][2];
  Result[2] = m[0] * Rotate[2][0] + m[1] * Rotate[2][1] + m[2] * Rotate[2][2];
  Result[3] = m[3];
  return Result;
}

} // namespace ft_glm
