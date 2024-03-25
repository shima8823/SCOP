#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>
#include <sstream>

#include "load.hpp"

std::string ReadShaderFromFile(const std::string &shaderPath) {
  std::ifstream shaderStream(shaderPath);
  if (!shaderStream.is_open()) {
    std::cerr << "Impossible to open " << shaderPath << ".\n";
    return "";
  }
  std::stringstream sstr;
  sstr << shaderStream.rdbuf();
  shaderStream.close();
  return sstr.str();
}

GLuint CompileShader(const std::string &path, GLenum shaderType) {
  std::string shaderCode = ReadShaderFromFile(path);
  if (shaderCode.empty())
    return 0;

  GLuint shaderID = glCreateShader(shaderType);
  std::cout << "Compiling shader: " << path << std::endl;
  char const *sourcePointer = shaderCode.c_str();
  glShaderSource(shaderID, 1, &sourcePointer, NULL);
  glCompileShader(shaderID);

  GLint result = GL_FALSE;
  int infoLogLength;
  glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
  glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);

  if (result == GL_FALSE) {
    std::vector<char> shaderErrorMessage(std::max(infoLogLength, 1));
    glGetShaderInfoLog(shaderID, infoLogLength, NULL, &shaderErrorMessage[0]);
    std::cerr << "Error compiling shader (" << path << "): "
              << std::string(shaderErrorMessage.begin(),
                             shaderErrorMessage.end())
              << "\n";
    glDeleteShader(shaderID);
    return 0;
  }

  if (infoLogLength > 0) {
    std::vector<char> shaderErrorMessage(infoLogLength + 1);
    glGetShaderInfoLog(shaderID, infoLogLength, NULL, &shaderErrorMessage[0]);
    std::cerr << "Shader compile log (" << path
              << "): " << &shaderErrorMessage[0] << "\n";
  }

  return shaderID;
}

GLuint load_shaders(const std::string &vertex_file_path,
                    const std::string &fragment_file_path) {
  GLuint vertexShaderID = CompileShader(vertex_file_path, GL_VERTEX_SHADER);
  GLuint fragmentShaderID =
      CompileShader(fragment_file_path, GL_FRAGMENT_SHADER);

  if (vertexShaderID == 0 || fragmentShaderID == 0) {
    return 0;
  }

  std::cout << "Linking program\n";
  GLuint programID = glCreateProgram();
  glAttachShader(programID, vertexShaderID);
  glAttachShader(programID, fragmentShaderID);
  glLinkProgram(programID);

  GLint result = GL_FALSE;
  int infoLogLength;
  glGetProgramiv(programID, GL_LINK_STATUS, &result);
  glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);

  if (result == GL_FALSE) {
    std::vector<char> programErrorMessage(std::max(infoLogLength, 1));
    glGetProgramInfoLog(programID, infoLogLength, NULL,
                        &programErrorMessage[0]);
    std::cerr << "Shader program linking failed: " << &programErrorMessage[0];

    glDeleteProgram(programID);
    glDetachShader(programID, vertexShaderID);
    glDetachShader(programID, fragmentShaderID);
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    return 0;
  }

  if (infoLogLength > 0) {
    std::vector<char> programErrorMessage(infoLogLength + 1);
    glGetProgramInfoLog(programID, infoLogLength, NULL,
                        &programErrorMessage[0]);
    std::cerr << "Shader program link log: " << &programErrorMessage[0];
  }

  glDetachShader(programID, vertexShaderID);
  glDetachShader(programID, fragmentShaderID);
  glDeleteShader(vertexShaderID);
  glDeleteShader(fragmentShaderID);

  return programID;
}