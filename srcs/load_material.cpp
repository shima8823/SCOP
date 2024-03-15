#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>
#include <sstream>

#include "load.hpp"

bool load_material(const std::string &path, Material &material) {
  std::cout << "Loading material " << path << "..." << std::endl;

  std::ifstream file("materials/" + path);
  if (!file.is_open()) {
    std::cerr << "Failed to open " << path << " file." << std::endl;
    return false;
  }

  std::string line;
  size_t lineCount = 0;
  while (std::getline(file, line)) {
    if (line.length() > MAX_LINE_LENGTH) {
      std::cerr << "Error: Line exceeds maximum length of " << MAX_LINE_LENGTH
                << " characters." << std::endl;
      return false;
    }
    if (lineCount > MAX_LINE_COUNT) {
      std::cerr << "Error: File exceeds maximum line count of "
                << MAX_LINE_COUNT << "." << std::endl;
      return false;
    }

    std::istringstream iss(line);
    std::string lineHeader;
    iss >> lineHeader;

    if (lineHeader == "Ns") {
      iss >> material.Ns;
    } else if (lineHeader == "Ka") {
      iss >> material.Ka.x >> material.Ka.y >> material.Ka.z;
    } else if (lineHeader == "Kd") {
      iss >> material.Kd.x >> material.Kd.y >> material.Kd.z;
    } else if (lineHeader == "Ks") {
      iss >> material.Ks.x >> material.Ks.y >> material.Ks.z;
    } else if (lineHeader == "Ni") {
      iss >> material.Ni;
    } else if (lineHeader == "d") {
      iss >> material.d;
    } else if (lineHeader == "illum") {
      iss >> material.illum;
    } else {
      continue;
    }
    if (iss.fail())
      std::cerr << "Material log: Failed to parse line: " << line << std::endl;
  }

  return true;
}