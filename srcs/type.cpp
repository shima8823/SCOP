#include <iostream>
#include "type.hpp"

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