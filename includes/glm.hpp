#pragma once

#include "type.hpp"

inline double radians(double degrees) { return degrees * (M_PI / 180.0); }
Mat4 perspective(float fov, float aspect, float zNear, float zFar);
Mat4 lookAt(const vec3 &eye, const vec3 &center, const vec3 &up);
