#pragma once

#include "ft_glm.hpp"
#include <vector>

bool loadOBJ(const char *path, std::vector<ft_glm::vec3> &out_vertices);
GLuint LoadShaders(const char *vertex_file_path,
                   const char *fragment_file_path);
