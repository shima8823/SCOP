#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>
#include <sstream>

#include "load.hpp"

std::vector<std::string> splitString(const std::string &input) {
  std::istringstream stream(input);
  std::vector<std::string> tokens;
  std::string token;

  while (stream >> token) {
    tokens.push_back(token);
  }

  return tokens;
}

// 文字列がどの形式か判定する関数
bool includeSlash(const std::string &input) {
  auto tokens = splitString(input);

  // 最初のトークンを除いた各トークンをチェック
  for (size_t i = 1; i < tokens.size(); ++i) {
    if (tokens[i].find('/') != std::string::npos) {
      // std::cout << "Format: f 1/1/1 2/2/2 3/3/3" << std::endl;
      return true;
    }
  }

  // std::cout << "Format: f 1 2 3 (or with more vertices)" << std::endl;
  return false;
}

bool loadOBJ(const char *path, std::vector<ft_glm::vec3> &out_vertices,
             std::vector<ft_glm::vec2> &out_uvs,
             std::vector<ft_glm::vec3> &out_normals, float limitsX[2],
             float limitsZ[2]) {
  printf("Loading OBJ file %s...\n", path);

  std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
  std::vector<ft_glm::vec3> temp_vertices;
  std::vector<ft_glm::vec2> temp_uvs;
  std::vector<ft_glm::vec3> temp_normals;

  FILE *file = fopen(path, "r");
  if (file == NULL) {
    printf("Impossible to open the file ! Are you in the right path ? See "
           "Tutorial 1 for details\n");
    getchar();
    return false;
  }

  while (1) {

    char lineHeader[128];
    // read the first word of the line
    int res = fscanf(file, "%s", lineHeader);
    if (res == EOF)
      break; // EOF = End Of File. Quit the loop.

    // else : parse lineHeader

    if (strcmp(lineHeader, "v") == 0) {
      ft_glm::vec3 vertex;
      fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
      temp_vertices.push_back(vertex);
      if (vertex.x < limitsX[0]) {
        limitsX[0] = vertex.x;
      }
      if (vertex.x > limitsX[1]) {
        limitsX[1] = vertex.x;
      }
      if (vertex.z < limitsZ[0]) {
        limitsZ[0] = vertex.z;
      }
      if (vertex.z > limitsZ[1]) {
        limitsZ[1] = vertex.z;
      }
    } else if (strcmp(lineHeader, "f") == 0) {
      /*
        f 1/1/1 2/2/2 3/3/3
        or
        f 1 2 3
        f 1 2 3 4
        f 1 2 3 4 5
      */

      std::vector<unsigned int> vertexN;
      unsigned int vertexIndex, uvIndex[3], normalIndex[3];

      char *line = NULL;
      size_t len = 0;
      getline(&line, &len, file);

      char *token = strtok(line, " ");
      size_t f_index = 0;
      if (includeSlash(line)) {
        std::cout << "includeSlash" << std::endl;
        while (token != NULL) {
          if (strcmp(token, "f") != 0) {
            if (f_index < 3) {
              sscanf(token, "%u", &vertexIndex);
              vertexN.push_back(vertexIndex);
            } else if (f_index < 6) {
              sscanf(token, "%u", &uvIndex[f_index - 3]);

            } else if (f_index < 9) {
              sscanf(token, "%u", &normalIndex[f_index - 6]);
            }
            f_index++;
          }
          token = strtok(NULL, " ");
        }

      } else {
        while (token != NULL) {
          // std::cout << "token = " << token;
          // std::cout << "f_index = " << f_index << std::endl;

          if (strcmp(token, "f") != 0) {
            sscanf(token, "%u", &vertexIndex);
            vertexN.push_back(vertexIndex);
          }
          token = strtok(NULL, " ");
        }
      }

      for (size_t i = 1; i + 1 < vertexN.size(); i++) {
        vertexIndices.push_back(vertexN[0]);
        vertexIndices.push_back(vertexN[i]);
        vertexIndices.push_back(vertexN[i + 1]);
      }
      if (f_index > 4) {
        uvIndices.push_back(uvIndex[0]);
        uvIndices.push_back(uvIndex[1]);
        uvIndices.push_back(uvIndex[2]);
      }
      if (f_index > 5) {
        normalIndices.push_back(normalIndex[0]);
        normalIndices.push_back(normalIndex[1]);
        normalIndices.push_back(normalIndex[2]);
      }

    } else if (strcmp(lineHeader, "vt") == 0) {
      ft_glm::vec2 uv;
      fscanf(file, "%f %f\n", &uv.x, &uv.y);

      temp_uvs.push_back(uv);
    } else if (strcmp(lineHeader, "vn") == 0) {
      ft_glm::vec3 normal;
      fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
      temp_normals.push_back(normal);
    } else {
      // Probably a comment, eat up the rest of the line
      char stupidBuffer[1000];
      fgets(stupidBuffer, 1000, file);
    }
  }

  // For each vertex of each triangle
  for (unsigned int i = 0; i < vertexIndices.size(); i++) {
    unsigned int vertexIndex = vertexIndices[i];
    // Get the attributes thanks to the index
    ft_glm::vec3 vertex = temp_vertices[vertexIndex - 1];

    // Put the attributes in buffers
    out_vertices.push_back(vertex);
  }

  std::cout << "vertexIndices.size() = " << vertexIndices.size() << std::endl;
  std::cout << "uvIndices.size() = " << uvIndices.size() << std::endl;
  std::cout << "normalIndices.size() = " << normalIndices.size() << std::endl;

  for (size_t i = 0; i < uvIndices.size(); i++) {
    unsigned int uvIndex = uvIndices[i];
    ft_glm::vec2 uv = temp_uvs[uvIndex - 1];
    out_uvs.push_back(uv);
  }

  std::cout << "temp_normals.size() = " << temp_normals.size() << std::endl;

  for (size_t i = 0; i < normalIndices.size(); i++) {
    unsigned int normalIndex = normalIndices[i];
    ft_glm::vec3 normal = temp_normals[normalIndex - 1];
    out_normals.push_back(normal);
  }

  fclose(file);
  return true;
}

GLuint LoadShaders(const char *vertex_file_path,
                   const char *fragment_file_path) {

  // Create the shaders
  GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
  GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

  // Read the Vertex Shader code from the file
  std::string VertexShaderCode;
  std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
  if (VertexShaderStream.is_open()) {
    std::stringstream sstr;
    sstr << VertexShaderStream.rdbuf();
    VertexShaderCode = sstr.str();
    VertexShaderStream.close();
  } else {
    printf("Impossible to open %s. Are you in the right directory ? Don't "
           "forget to read the FAQ !\n",
           vertex_file_path);
    getchar();
    return 0;
  }

  // Read the Fragment Shader code from the file
  std::string FragmentShaderCode;
  std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
  if (FragmentShaderStream.is_open()) {
    std::stringstream sstr;
    sstr << FragmentShaderStream.rdbuf();
    FragmentShaderCode = sstr.str();
    FragmentShaderStream.close();
  }

  GLint Result = GL_FALSE;
  int InfoLogLength;

  // Compile Vertex Shader
  printf("Compiling shader : %s\n", vertex_file_path);
  char const *VertexSourcePointer = VertexShaderCode.c_str();
  glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
  glCompileShader(VertexShaderID);

  // Check Vertex Shader
  glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if (InfoLogLength > 0) {
    std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
    glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL,
                       &VertexShaderErrorMessage[0]);
    printf("%s\n", &VertexShaderErrorMessage[0]);
  }

  // Compile Fragment Shader
  printf("Compiling shader : %s\n", fragment_file_path);
  char const *FragmentSourcePointer = FragmentShaderCode.c_str();
  glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
  glCompileShader(FragmentShaderID);

  // Check Fragment Shader
  glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if (InfoLogLength > 0) {
    std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL,
                       &FragmentShaderErrorMessage[0]);
    printf("%s\n", &FragmentShaderErrorMessage[0]);
  }

  // Link the program
  printf("Linking program\n");
  GLuint ProgramID = glCreateProgram();
  glAttachShader(ProgramID, VertexShaderID);
  glAttachShader(ProgramID, FragmentShaderID);
  glLinkProgram(ProgramID);

  // Check the program
  glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
  glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if (InfoLogLength > 0) {
    std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL,
                        &ProgramErrorMessage[0]);
    printf("%s\n", &ProgramErrorMessage[0]);
  }

  glDetachShader(ProgramID, VertexShaderID);
  glDetachShader(ProgramID, FragmentShaderID);

  glDeleteShader(VertexShaderID);
  glDeleteShader(FragmentShaderID);

  return ProgramID;
}

GLuint loadBMP_custom(const char *imagepath) {

  printf("Reading image %s\n", imagepath);

  // Data read from the header of the BMP file
  unsigned char header[54];
  unsigned int dataPos;
  unsigned int imageSize;
  unsigned int width, height;
  // Actual RGB data
  unsigned char *data;

  // Open the file
  FILE *file = fopen(imagepath, "rb");
  if (!file) {
    printf("%s could not be opened. Are you in the right directory ? Don't "
           "forget to read the FAQ !\n",
           imagepath);
    getchar();
    return 0;
  }

  // Read the header, i.e. the 54 first bytes

  // If less than 54 bytes are read, problem
  if (fread(header, 1, 54, file) != 54) {
    printf("Not a correct BMP file 1\n");
    fclose(file);
    return 0;
  }
  // A BMP files always begins with "BM"
  if (header[0] != 'B' || header[1] != 'M') {
    printf("Not a correct BMP file 2\n");
    fclose(file);
    return 0;
  }
  // Make sure this is a 24bpp file
  if (*(int *)&(header[0x1E]) != 0) {
    printf("Not a correct BMP file 3\n");
    fclose(file);
    return 0;
  }
  if (*(int *)&(header[0x1C]) != 24) {
    printf("Not a correct BMP file 4\n");
    fclose(file);
    return 0;
  }

  // Read the information about the image
  dataPos = *(int *)&(header[0x0A]);
  imageSize = *(int *)&(header[0x22]);
  width = *(int *)&(header[0x12]);
  height = *(int *)&(header[0x16]);

  // Some BMP files are misformatted, guess missing information
  if (imageSize == 0)
    imageSize = width * height *
                3; // 3 : one byte for each Red, Green and Blue component
  if (dataPos == 0)
    dataPos = 54; // The BMP header is done that way

  // Create a buffer
  data = new unsigned char[imageSize];

  // Read the actual data from the file into the buffer
  fread(data, 1, imageSize, file);

  // Everything is in memory now, the file can be closed.
  fclose(file);

  // Create one OpenGL texture
  GLuint textureID;
  glGenTextures(1, &textureID);

  // "Bind" the newly created texture : all future texture functions will modify
  // this texture
  glBindTexture(GL_TEXTURE_2D, textureID);

  // Give the image to OpenGL
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR,
               GL_UNSIGNED_BYTE, data);

  // OpenGL has now copied the data. Free our own version
  delete[] data;

  // Poor filtering, or ...
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  // ... nice trilinear filtering ...
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  // ... which requires mipmaps. Generate them automatically.
  glGenerateMipmap(GL_TEXTURE_2D);

  // Return the ID of the texture we just created
  return textureID;
}