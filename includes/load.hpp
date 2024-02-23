#pragma once

#include "ft_glm.hpp"
#include <vector>

bool loadOBJ(const char *path, std::vector<ft_glm::vec3> &out_vertices, std::vector<ft_glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals, float limitsX[2], float limitsZ[2]);
GLuint LoadShaders(const char *vertex_file_path,
                   const char *fragment_file_path);
GLuint loadBMP_custom(const char * imagepath);
