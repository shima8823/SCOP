#include "ft_glm.hpp"
#include <iostream>

namespace ft_glm {

Mat4::Mat4(float defaulValue) {
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      mat[i][j] = (i == j) ? defaulValue : 0.0f;
    }
  }
}

Mat4 Mat4::operator+(const Mat4 &other) const {
  Mat4 result(0.0f);
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      result[i][j] = mat[i][j] + other.mat[i][j];
    }
  }
  return result;
}

Mat4 Mat4::operator-(const Mat4 &other) const {
  Mat4 result(0.0f);
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      result[i][j] = mat[i][j] - other.mat[i][j];
    }
  }
  return result;
}

Mat4 Mat4::operator*(float scalar) const {
  Mat4 result(0.0f);
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      result[i][j] = mat[i][j] * scalar;
    }
  }
  return result;
}

Mat4 Mat4::operator*(const Mat4 &other) const {
  Mat4 result(0.0f);
  for (int i = 0; i < 4; ++i) {   // 結果行列の各列に対して
    for (int j = 0; j < 4; ++j) { // 結果行列の各行に対して
      for (int k = 0; k < 4; ++k) {
        // 左側の行列のk列目と右側の行列のk行目のドット積
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

//   result[2][2] = zFar / (zFar - zNear);
//   result[3][2] = (-zFar * zNear) / (zFar - zNear);


  result[2][3] = -1.0f;

  return result;
}

Mat4 lookAt(const vec3 &eye, const vec3 &center, const vec3 &up) {
// f,s,u is vector. why they having vec3 because ワールド座標にするため
  vec3 f = (center - eye).normalize(); // forward vector
  vec3 s = f.cross(up).normalize();    // side vector
  vec3 u = s.cross(f);                 // corrected up vector

  Mat4 result(1.0f);
  result[0][0] = s.x;
  result[1][0] = s.y;
  result[2][0] = s.z;
  result[0][1] = u.x;
  result[1][1] = u.y;
  result[2][1] = u.z;

//   なぜマイナスかは、z軸が逆だから
  result[0][2] = -f.x;
  result[1][2] = -f.y;
  result[2][2] = -f.z;

// これは何表してんの？
  result[3][0] = -s.dot(eye);
  result[3][1] = -u.dot(eye);
  result[3][2] = f.dot(eye);

  return result;
}
} // namespace ft_glm