#pragma once

#include "ft_glm.hpp"
#include <vector>

// file limits
const unsigned int MAX_LINE_LENGTH = 1024;
const unsigned int MAX_LINE_COUNT = 100000;

// bmp file
const unsigned int MAX_FILE_SIZE = 4096;

struct Material {
  float Ns;
  ft_glm::vec3 Ka;
  ft_glm::vec3 Kd;
  ft_glm::vec3 Ks;
  float Ni;
  float d;
  int illum;
};

bool load_material(const std::string &path, Material &material);
bool load_object(const std::string &path,
                 std::vector<ft_glm::vec3> &out_vertices,
                 std::vector<ft_glm::vec2> &out_uvs,
                 std::vector<ft_glm::vec3> &out_normals, float limitsX[2],
                 float limitsZ[2], std::string &materialFilename);
GLuint load_shaders(const std::string &vertex_file_path,
                    const std::string &fragment_file_path);
GLuint load_bmp(const std::string &path);