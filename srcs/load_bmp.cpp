#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>
#include <sstream>

#include "load.hpp"

GLuint load_bmp(const std::string &path) {
  std::cout << "Loading bmp " << path << "..." << std::endl;

  std::ifstream file(path, std::ios::binary);
  if (!file.is_open()) {
    std::cerr << path << "Failed to open." << std::endl;
    return 0;
  }

  unsigned char header[54];
  file.read(reinterpret_cast<char *>(header), 54);
  if (file.gcount() != 54 || header[0] != 'B' || header[1] != 'M') {
    std::cerr << "Not a correct BMP file\n";
    return 0;
  }

  // reinterpret_cast is used to convert the pointer to a different type
  unsigned int dataPos = *reinterpret_cast<unsigned int *>(&header[0x0A]);
  unsigned int imageSize = *reinterpret_cast<unsigned int *>(&header[0x22]);
  unsigned int width = *reinterpret_cast<unsigned int *>(&header[0x12]);
  unsigned int height = *reinterpret_cast<unsigned int *>(&header[0x16]);

  if (width > MAX_FILE_SIZE || height > MAX_FILE_SIZE) {
    std::cerr << "BMP file is too large: " << width << "x" << height
              << std::endl;
    file.close();
    return 0;
  }

  if (imageSize == 0)
    imageSize = width * height * 3;
  if (dataPos == 0)
    dataPos = 54;

  std::vector<unsigned char> data(imageSize);

  file.seekg(dataPos);
  file.read(reinterpret_cast<char *>(data.data()), imageSize);
  file.close();

  GLuint textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR,
               GL_UNSIGNED_BYTE, data.data());

  // Set texture parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glGenerateMipmap(GL_TEXTURE_2D);

  return textureID;
}