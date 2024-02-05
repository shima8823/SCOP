#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>

#include "load.hpp"

bool loadOBJ(const char *path, std::vector<vec3> &out_vertices) {
  printf("Loading OBJ file %s...\n", path);

  std::vector<unsigned int> vertexIndices;
  std::vector<vec3> temp_vertices;

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
      vec3 vertex;
      fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
      temp_vertices.push_back(vertex);
    } else if (strcmp(lineHeader, "f") == 0) {
      std::vector<unsigned int> vertexN;
      unsigned int vertexIndex;

      char *line = NULL;
      size_t len = 0;
      getline(&line, &len, file);

      char *token = strtok(line, " ");
      while (token != NULL) {
        if (strcmp(token, "f") != 0) {
          sscanf(token, "%u", &vertexIndex);
          vertexN.push_back(vertexIndex);
        }
        token = strtok(NULL, " ");
      }

      for (size_t i = 1; i + 1 < vertexN.size(); i++) {
        vertexIndices.push_back(vertexN[0]);
        vertexIndices.push_back(vertexN[i]);
        vertexIndices.push_back(vertexN[i + 1]);
      }

    } else {
      // Probably a comment, eat up the rest of the line
      char stupidBuffer[1000];
      fgets(stupidBuffer, 1000, file);
    }
  }

  // For each vertex of each triangle
  for (unsigned int i = 0; i < vertexIndices.size(); i++) {

    // Get the indices of its attributes
    unsigned int vertexIndex = vertexIndices[i];

    // Get the attributes thanks to the index
    vec3 vertex = temp_vertices[vertexIndex - 1];

    // Put the attributes in buffers
    out_vertices.push_back(vertex);
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
