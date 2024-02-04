#include "glm.hpp"

Mat4 perspective(float fov, float aspect, float zNear, float zFar) {
  const float tanHalfFovy = tan(fov / 2.0f);

  Mat4 result(0.0f);
  result[0][0] = 1.0f / (aspect * tanHalfFovy);
  result[1][1] = 1.0f / (tanHalfFovy);
  result[2][2] = -(zFar + zNear) / (zFar - zNear);
  result[2][3] = -1.0f;
  result[3][2] = -(2 * zFar * zNear) / (zFar - zNear);

  return result;
}

Mat4 lookAt(const vec3 &eye, const vec3 &center, const vec3 &up) {
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
  result[0][2] = -f.x;
  result[1][2] = -f.y;
  result[2][2] = -f.z;
  result[3][0] = -s.dot(eye);
  result[3][1] = -(u.dot(eye));
  result[3][2] = f.dot(eye);

  return result;
}
