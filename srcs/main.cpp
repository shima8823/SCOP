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

float getRandom() {
  std::random_device rd; // ランダムなシードを生成するためのデバイス
  std::mt19937 gen(rd()); // メルセンヌ・ツイスターという乱数生成器
  std::uniform_real_distribution<float> dis(0.0,
                                            1.0); // 0.0から1.0までの一様分布

  return dis(gen);
}

bool init() {
  // GLFWライブラリの初期化
  if (!glfwInit())
    return false;

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  window =
      glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "scop", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return false;
  }

  glfwMakeContextCurrent(window);
  glEnable(GL_DEPTH_TEST);

  glewExperimental = true; // Needed for core profile
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    getchar();
    glfwTerminate();
    return false;
  }

  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
  glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
  return true;
}

void calculateNormals(std::vector<ft_glm::vec3> &vertices,
                      std::vector<ft_glm::vec3> &out_normals) {

  out_normals.resize(vertices.size(), glm::vec3(0.0f, 0.0f, 0.0f));

  if (vertices.empty() || vertices.size() % 3 != 0) {
    std::cerr << "Invalid vertex list." << std::endl;
    return;
  }

  out_normals.resize(vertices.size(), glm::vec3(0.0f, 0.0f, 0.0f));

  for (size_t i = 0; i < vertices.size(); i += 3) {
    ft_glm::vec3 v0 = vertices[i];
    ft_glm::vec3 v1 = vertices[i + 1];
    ft_glm::vec3 v2 = vertices[i + 2];

    // 面の法線を計算
    // ft_glm::vec3 normal = ft_glm::normalize(ft_glm::cross(v1 - v0, v2 - v0));
    ft_glm::vec3 normal = ft_glm::cross(v1 - v0, v2 - v0);
    normal.normalize();

    // 各頂点に対する法線を更新
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
  glVertexAttribPointer(1,
                        3,        // size
                        GL_FLOAT, // type
                        GL_FALSE, // normalized?
                        0,        // stride
                        (void *)0 // array buffer offset
  );

  glEnableVertexAttribArray(2);
  glBindBuffer(GL_ARRAY_BUFFER, vbo.uv);
  glVertexAttribPointer(2,
                        2,        // size : U+V => 2
                        GL_FLOAT, // type
                        GL_FALSE, // normalized?
                        0,        // stride
                        (void *)0 // array buffer offset
  );

  glEnableVertexAttribArray(3);
  glBindBuffer(GL_ARRAY_BUFFER, vbo.normal);
  glVertexAttribPointer(3,
                        3,        // size : U+V => 2
                        GL_FLOAT, // type
                        GL_FALSE, // normalized?
                        0,        // stride
                        (void *)0 // array buffer offset
  );
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

GLuint setupTexture(const int argc, const char *argv[], GLuint useTextureLocation) {
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

  sendUniforms(programID, material);

  // texture
  GLuint useTextureLocation = glGetUniformLocation(programID, "useTexture");

  GLuint Texture = setupTexture(argc, argv, useTextureLocation);

  std::vector<ft_glm::vec2> g_uv_buffer_data;
  std::vector<ft_glm::vec3> g_color_buffer_data;

  if (uvs.size() == 0) {
    for (int i = 0; i < vertices.size(); i++) {
      g_uv_buffer_data.push_back(ft_glm::vec2(0.0f, 0.0f));
      g_uv_buffer_data.push_back(ft_glm::vec2(0.0f, 1.0f));
      g_uv_buffer_data.push_back(ft_glm::vec2(1.0f, 1.0f));
    }
  } else {
    for (int i = 0; i < uvs.size(); i++)
      g_uv_buffer_data.push_back(ft_glm::vec2(uvs[i].x, uvs[i].y));
  }

  if (normals.size() == 0) {
    calculateNormals(vertices, normals);
  }

  for (int i = 0; i < vertices.size(); i++) {
    g_color_buffer_data.push_back(
        ft_glm::vec3(getRandom(), getRandom(), getRandom()));
  }

  GLuint MatrixID      = glGetUniformLocation(programID, "MVP");
  GLuint ModelMatrixID = glGetUniformLocation(programID, "model");
  GLuint LightPosID    = glGetUniformLocation(programID, "lightPos");

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPos(window, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

  // メインループのための変数
  double lastTime = glfwGetTime();
  double lastFrameTime = lastTime;
  int nbFrames = 0;

  float initialPositionX = (limitsX[0] + limitsX[1]) / 2.0f;
  float initialPositionZ = (limitsZ[0] + limitsZ[1]) / 2.0f;
  glm::vec3 gPosition1(initialPositionX, 0.0f, initialPositionZ);
  glm::vec3 gOrientation1(0.0f, 0.0f, 0.0f);
  GLuint             vao;
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
    nbFrames++;
    if (currentTime - lastTime >=
        1.0) { // If last prinf() was more than 1sec ago
      // printf and reset
      printf("%f ms/frame\n", 1000.0 / double(nbFrames));
      nbFrames = 0;
      lastTime += 1.0;
    }

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

    gOrientation1.y += 3.14159f / 2.0f * deltaTime;

    glm::mat4 TranslationMatrix = glm::translate(glm::mat4(1.0f), -gPosition1);
    // glm::mat4 RotationMatrix = glm::rotate(glm::mat4(1.0f), gOrientation1.y,
    // glm::vec3(0,1,0));
    glm::mat4 RotationMatrix =
        glm::rotate(glm::mat4(1.0f), gOrientation1.y, gOrientation1);
    glm::mat4 BackTranslationMatrix =
        glm::translate(glm::mat4(1.0f), gPosition1);
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
