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
