#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>
#include <sstream>

#include "load.hpp"

const size_t MAX_LINE_LENGTH = 1024;
const size_t MAX_LINE_COUNT = 100000;

std::vector<std::string> split(const std::string &s, char delimiter) {
  std::vector<std::string> tokens;
  std::string token;
  std::istringstream tokenStream(s);
  while (std::getline(tokenStream, token, delimiter)) {
    tokens.push_back(token);
  }
  return tokens;
}

bool parseVertex(const std::string &line, ft_glm::vec3 &vertex,
                 float limitsX[2], float limitsZ[2]) {
  std::istringstream iss(line);
  std::string prefix;
  iss >> prefix;
  if (!(iss >> vertex.x >> vertex.y >> vertex.z)) {
    std::cerr << "Error: Failed to parse vertex coordinates from line: " << line
              << std::endl;
    return false;
  }
  limitsX[0] = std::min(limitsX[0], vertex.x);
  limitsX[1] = std::max(limitsX[1], vertex.x);
  limitsZ[0] = std::min(limitsZ[0], vertex.z);
  limitsZ[1] = std::max(limitsZ[1], vertex.z);
  return true;
}

bool parseFace(const std::vector<std::string> &tokens, bool isOnlyVertex,
               std::vector<unsigned int> &vertexIndices,
               std::vector<unsigned int> &uvIndices,
               std::vector<unsigned int> &normalIndices) {
  std::vector<unsigned int> vertexN, uvN, normalN;
  try {
    for (const std::string &token : tokens) {
      std::vector<std::string> subtokens = split(token, '/');
      if (subtokens.empty())
        throw std::invalid_argument("Empty token.");

      unsigned int vIndex = std::stoi(subtokens[0]);
      unsigned int uvIndex = subtokens.size() > 1 && !subtokens[1].empty()
                                 ? std::stoi(subtokens[1])
                                 : 0;
      unsigned int nIndex = subtokens.size() > 2 && !subtokens[2].empty()
                                ? std::stoi(subtokens[2])
                                : 0;

      vertexN.push_back(vIndex);
      if (!isOnlyVertex) {
        uvN.push_back(uvIndex);
        normalN.push_back(nIndex);
      }
    }

    for (size_t i = 1; i + 1 < vertexN.size(); ++i) {
      vertexIndices.push_back(vertexN[0]);
      vertexIndices.push_back(vertexN[i]);
      vertexIndices.push_back(vertexN[i + 1]);
      if (!isOnlyVertex) {
        uvIndices.push_back(uvN[0]);
        uvIndices.push_back(uvN[i]);
        uvIndices.push_back(uvN[i + 1]);
        normalIndices.push_back(normalN[0]);
        normalIndices.push_back(normalN[i]);
        normalIndices.push_back(normalN[i + 1]);
      }
    }
  } catch (const std::invalid_argument &e) {
    std::cerr << "Invalid argument during face parsing: " << e.what()
              << std::endl;
    return false;
  } catch (const std::out_of_range &e) {
    std::cerr << "Value out of range during face parsing: " << e.what()
              << std::endl;
    return false;
  } catch (...) {
    std::cerr << "Unknown error during face parsing." << std::endl;
    return false;
  }

  return true;
}

bool parseUV(const std::string &line, ft_glm::vec2 &uv) {
  std::istringstream iss(line);
  std::string prefix;
  iss >> prefix;
  if (!(iss >> uv.x >> uv.y)) {
    std::cerr << "Error: Failed to parse UV coordinates from line: " << line
              << std::endl;
    return false;
  }
  return true;
}

bool parseNormal(const std::string &line, ft_glm::vec3 &normal) {
  std::istringstream iss(line);
  std::string prefix;
  iss >> prefix;
  if (!(iss >> normal.x >> normal.y >> normal.z)) {
    std::cerr << "Error: Failed to parse normal coordinates from line: " << line
              << std::endl;
    return false;
  }
  return true;
}

bool loadOBJ(const char *path, std::vector<ft_glm::vec3> &out_vertices,
             std::vector<ft_glm::vec2> &out_uvs,
             std::vector<ft_glm::vec3> &out_normals, float limitsX[2],
             float limitsZ[2], std::string &materialFilename) {
  std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
  std::vector<ft_glm::vec3> temp_vertices;
  std::vector<ft_glm::vec2> temp_uvs;
  std::vector<ft_glm::vec3> temp_normals;

  std::cout << "Loading OBJ file " << path << "...\n";

  std::ifstream file(path);
  if (!file.is_open())
    return false;

  limitsX[0] = std::numeric_limits<float>::max();
  limitsX[1] = std::numeric_limits<float>::lowest();
  limitsZ[0] = std::numeric_limits<float>::max();
  limitsZ[1] = std::numeric_limits<float>::lowest();

  std::string line;
  size_t lineCount = 0;
  while (std::getline(file, line)) {
    lineCount++;
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

    if (lineHeader == "v") {
      ft_glm::vec3 vertex;
      if (!parseVertex(line, vertex, limitsX, limitsZ))
        return false;
      temp_vertices.push_back(vertex);
    } else if (lineHeader == "vt") {
      ft_glm::vec2 uv;
      if (!parseUV(line, uv))
        return false;
      temp_uvs.push_back(uv);
    } else if (lineHeader == "vn") {
      ft_glm::vec3 normal;
      if (!parseNormal(line, normal))
        return false;
      temp_normals.push_back(normal);
    } else if (lineHeader == "f") {
      /*
        f 1/1/1 2/2/2 3/3/3
        f 1/1/1 2/2/2 3/3/3 4/4/4
        or
        f 1 2 3
        f 1 2 3 4
        f 1 2 3 4 5
      */
      std::string token;
      std::vector<std::string> tokens;
      bool isOnlyVertex = line.find('/') == std::string::npos;

      while (iss >> token) {
        tokens.push_back(token);
      }

      if (!parseFace(tokens, isOnlyVertex, vertexIndices, uvIndices,
                     normalIndices))
        return false;
    } else if (lineHeader == "mtllib") {
      iss >> materialFilename;
    }
  }

  for (size_t i = 0; i < vertexIndices.size(); i++)
    out_vertices.push_back(temp_vertices[vertexIndices[i] - 1]);
  for (size_t i = 0; i < uvIndices.size(); i++)
    out_uvs.push_back(temp_uvs[uvIndices[i] - 1]);
  for (size_t i = 0; i < normalIndices.size(); i++)
    out_normals.push_back(temp_normals[normalIndices[i] - 1]);

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

bool loadMaterial(const std::string path, Material &material) {
  std::cout << "Loading material " << path << "...\n";

  std::ifstream file("materials/" + path);
  if (!file.is_open()) {
    std::cout << "Impossible to open the file! Are you in the right path?\n";
    return false;
  }

  std::string line;

  while (std::getline(file, line)) {
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
    }
  }

  return true;
}
