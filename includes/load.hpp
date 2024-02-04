#pragma once

#include "type.hpp"
#include <vector>

bool loadOBJ(const char *path, std::vector<vec3> &out_vertices);
GLuint LoadShaders(const char *vertex_file_path,
                   const char *fragment_file_path);
