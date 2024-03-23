#include "opengl.hpp"
#include <vector>

#include <cmath>

#include "ft_glm.hpp"
#include "load.hpp"

#include <iostream>

#include "controls.hpp"

using namespace std;

#include "const.hpp"
#include <random>

GLFWwindow *window;

const float ROTATE_SPEED = M_PI / 2.0f; // 90 degrees per second

bool init() {
  if (!glfwInit())
    return false;

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "scop", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return false;
  }
  glfwMakeContextCurrent(window);
  glEnable(GL_DEPTH_TEST);
  if (glewInit() != GLEW_OK) {
    std::cerr << "Failed to initialize GLEW" << std::endl;
    getchar();
    glfwTerminate();
    return false;
  }
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
  glClearColor(0.25f, 0.25f, 0.25f, 0.0f);
  return true;
}

float getRandom() {
  static std::random_device rd; // ランダムなシードを生成するためのデバイス
  static std::mt19937 gen(rd()); // メルセンヌ・ツイスターという乱数生成器
  static std::uniform_real_distribution<float> dis(
      0.0,
      1.0); // 0.0から1.0までの一様分布

  return dis(gen);
}

void calculateNormals(const std::vector<ft_glm::vec3> &vertices,
                      std::vector<ft_glm::vec3> &out_normals) {
  if (vertices.empty() || vertices.size() % 3 != 0) {
    std::cerr << "Invalid vertex list." << std::endl;
    return;
  }
  out_normals.resize(vertices.size(), ft_glm::vec3(0.0f, 0.0f, 0.0f));

  for (size_t i = 0; i < vertices.size(); i += 3) {
    ft_glm::vec3 v0 = vertices[i];
    ft_glm::vec3 v1 = vertices[i + 1];
    ft_glm::vec3 v2 = vertices[i + 2];

    ft_glm::vec3 normal = ft_glm::cross(v1 - v0, v2 - v0);
    normal.normalize();

    out_normals[i] = out_normals[i + 1] = out_normals[i + 2] = normal;
  }
}

void setupVertexBuffers(Vertexbufferobject &vbo,
                        std::vector<ft_glm::vec3> &vertices,
                        std::vector<ft_glm::vec2> &uvs,
                        std::vector<ft_glm::vec3> &normals,
                        std::vector<ft_glm::vec3> &colors) {
  glGenBuffers(1, &vbo.vertex);
  glBindBuffer(GL_ARRAY_BUFFER, vbo.vertex);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(ft_glm::vec3),
               &vertices[0], GL_STATIC_DRAW);

  glGenBuffers(1, &vbo.uv);
  glBindBuffer(GL_ARRAY_BUFFER, vbo.uv);
  glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(ft_glm::vec2), &uvs[0],
               GL_STATIC_DRAW);

  glGenBuffers(1, &vbo.normal);
  glBindBuffer(GL_ARRAY_BUFFER, vbo.normal);
  glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(ft_glm::vec3),
               &normals[0], GL_STATIC_DRAW);

  glGenBuffers(1, &vbo.color);
  glBindBuffer(GL_ARRAY_BUFFER, vbo.color);
  glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(ft_glm::vec3),
               &colors[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vbo.vertex);
  glVertexAttribPointer(0,
                        3,        // size
                        GL_FLOAT, // type
                        GL_FALSE, // normalized?
                        0,        // stride
                        (void *)0 // array buffer offset
  );

  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, vbo.color);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

  glEnableVertexAttribArray(2);
  glBindBuffer(GL_ARRAY_BUFFER, vbo.uv);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);

  glEnableVertexAttribArray(3);
  glBindBuffer(GL_ARRAY_BUFFER, vbo.normal);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

GLuint setupTexture(const int argc, const char *argv[],
                    GLuint useTextureLocation) {
  GLuint Texture = 0;
  if (argc > 2) {
    glUniform1i(useTextureLocation, 1);
    Texture = load_bmp(argv[2]);
    if (Texture == 0) {
      std::cerr << "Error: Texture failed to load." << std::endl;
    } else {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, Texture);
    }
  } else {
    glUniform1i(useTextureLocation, 0);
  }
  return Texture;
}

void fillBufferVectors(const std::vector<ft_glm::vec3> &vertices,
                       const std::vector<ft_glm::vec2> &uvs,
                       std::vector<ft_glm::vec3> &normals,
                       std::vector<ft_glm::vec2> &uvBuffer,
                       std::vector<ft_glm::vec3> &colorBuffer) {
  if (uvs.empty()) {
    for (size_t i = 0; i < vertices.size(); i++) {
      uvBuffer.emplace_back(0.0f, 0.0f);
      uvBuffer.emplace_back(0.0f, 1.0f);
      uvBuffer.emplace_back(1.0f, 1.0f);
    }
  } else {
    uvBuffer.assign(uvs.begin(), uvs.end());
  }

  if (normals.empty()) {
    calculateNormals(vertices, normals);
  }

  for (size_t i = 0; i < vertices.size(); ++i) {
    colorBuffer.emplace_back(getRandom(), getRandom(), getRandom());
  }
}

void sendUniforms(GLuint programID, Material &material) {
  GLuint ambientLocation = glGetUniformLocation(programID, "ambientMaterial");
  GLuint diffuseLocation = glGetUniformLocation(programID, "diffuseMaterial");
  GLuint specularLocation = glGetUniformLocation(programID, "specularMaterial");
  GLuint shininessLocation = glGetUniformLocation(programID, "shininess");

  glUniform3f(ambientLocation, material.Ka.x, material.Ka.y, material.Ka.z);
  glUniform3f(diffuseLocation, material.Kd.x, material.Kd.y, material.Kd.z);
  glUniform3f(specularLocation, material.Ks.x, material.Ks.y, material.Ks.z);
  glUniform1f(shininessLocation, material.Ns);
}

int main(const int argc, const char *argv[]) {
  if (!init()) {
    return -1;
  }

  GLuint programID = load_shaders("shaders/VertexShader.vertexshader",
                                  "shaders/FragmentShader.fragmentshader");
  if (programID == 0) {
    std::cerr << "Error: Shader program failed to load." << std::endl;
    exit(EXIT_FAILURE);
  }
  glUseProgram(programID);

  std::vector<ft_glm::vec3> vertices;
  std::vector<ft_glm::vec2> uvs;
  std::vector<ft_glm::vec3> normals;
  std::string materialFilename;
  float limitsX[2] = {0, 0};
  float limitsZ[2] = {0, 0};
  bool res = load_object(argv[1], vertices, uvs, normals, limitsX, limitsZ,
                         materialFilename);
  if (!res) {
    std::cout << "Error: Object file failed to load." << std::endl;
    exit(EXIT_FAILURE);
  }

  Material material;
  res = load_material(materialFilename, material);
  if (!res) {
    std::cout << "Failed to load material" << std::endl;
  }

  std::vector<ft_glm::vec2> g_uv_buffer_data;
  std::vector<ft_glm::vec3> g_color_buffer_data;

  fillBufferVectors(vertices, uvs, normals, g_uv_buffer_data,
                    g_color_buffer_data);
  sendUniforms(programID, material);

  GLuint useTextureLocation = glGetUniformLocation(programID, "useTexture");
  GLuint isRainbowLocation = glGetUniformLocation(programID, "isRainbow");
  GLuint Texture = setupTexture(argc, argv, useTextureLocation);
  GLuint MatrixID = glGetUniformLocation(programID, "MVP");
  GLuint ModelMatrixID = glGetUniformLocation(programID, "model");
  GLuint LightPosID = glGetUniformLocation(programID, "lightPos");

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPos(window, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

  double lastTime = glfwGetTime();
  double lastFrameTime = lastTime;
  float initialPositionX = (limitsX[0] + limitsX[1]) / 2.0f;
  float initialPositionZ = (limitsZ[0] + limitsZ[1]) / 2.0f;
  ft_glm::vec3 gPosition1(initialPositionX, 0.0f, initialPositionZ);
  float angle = 0.0f;
  GLuint vao;
  Vertexbufferobject vbo;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  setupVertexBuffers(vbo, vertices, g_uv_buffer_data, normals,
                     g_color_buffer_data);
  glBindVertexArray(0);
  do {

    double currentTime = glfwGetTime();
    float deltaTime = (float)(currentTime - lastFrameTime);
    lastFrameTime = currentTime;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindVertexArray(vao);

    computeMatricesFromInputs(window);
    ft_glm::Mat4 ProjectionMatrix = getProjectionMatrix();
    ft_glm::Mat4 ViewMatrix = getViewMatrix();
    if (getTexture()) {
      glUniform1i(useTextureLocation, 1);
    } else {
      glUniform1i(useTextureLocation, 0);
    }

    if ((getIsRainbow())) {
      glUniform1i(isRainbowLocation, 1);
    } else {
      glUniform1i(isRainbowLocation, 0);
    }

    ft_glm::vec3 rotationAxis = getRotationAxis();
    angle += ROTATE_SPEED * deltaTime;

    ft_glm::Mat4 TranslationMatrix =
        ft_glm::translate(ft_glm::Mat4(1.0f), -gPosition1);
    ft_glm::Mat4 RotationMatrix =
        ft_glm::rotate(ft_glm::Mat4(1.0f), angle, rotationAxis);
    ft_glm::Mat4 BackTranslationMatrix =
        ft_glm::translate(ft_glm::Mat4(1.0f), gPosition1);
    ft_glm::Mat4 ModelMatrix =
        BackTranslationMatrix * RotationMatrix * TranslationMatrix;

    ft_glm::Mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

    ft_glm::vec3 cameraPos = getPosition();
    glUniform3f(LightPosID, cameraPos.x, cameraPos.y, cameraPos.z);

    glDrawArrays(GL_TRIANGLES, 0, vertices.size());

    glfwSwapBuffers(window);
    glfwPollEvents();
  } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);

  // Cleanup VBO
  glDeleteBuffers(1, &vbo.vertex);
  glDeleteBuffers(1, &vbo.color);
  glDeleteBuffers(1, &vbo.uv);
  glDeleteBuffers(1, &vbo.normal);

  // Cleanup VAO
  glDeleteVertexArrays(1, &vao);

  glDeleteTextures(1, &Texture);
  glDeleteProgram(programID);

  glfwTerminate();
  return 0;
}
