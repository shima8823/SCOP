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
  std::cout << "I'm apple machine" << std::endl;
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  // ウィンドウの作成
  window =
      glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello World", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return false;
  }

  //   oepngl shader version: 4.1
  glfwMakeContextCurrent(window);
  glEnable(GL_DEPTH_TEST);

  // Initialize GLEW
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

int main(const int argc, const char *argv[]) {
  if (!init()) {
    return -1;
  }
  GLuint VertexArrayID;
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);

  // Read our .obj file
  std::vector<ft_glm::vec3> vertices;
  std::vector<ft_glm::vec2> uvs;
  std::vector<ft_glm::vec3> normals;
  float limitsX[2] = {0, 0};
  float limitsZ[2] = {0, 0};
  bool res = loadOBJ(argv[1], vertices, uvs, normals, limitsX, limitsZ);

  if (!res) {
    std::cout << "failed to load obj" << std::endl;
    return -1;
  }

  // shader program
  GLuint programID = LoadShaders("shaders/VertexShader.vertexshader",
                                 "shaders/FragmentShader.fragmentshader");

  glUseProgram(programID);

  GLuint useTextureLocation = glGetUniformLocation(programID, "useTexture");
  GLuint Texture;
  if (argc > 2) {
    glUniform1i(useTextureLocation, 0);
    // テクスチャのバインドやその他の設定
    Texture = loadBMP_custom(argv[2]);
  } else {
    glUniform1i(useTextureLocation, 10);
  }

  // Get a handle for our "myTextureSampler" uniform
  GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");

  std::vector<glm::vec2> g_uv_buffer_data;

  if (uvs.size() == 0) {
    std::cout << "no uvs" << std::endl;
    for (int i = 0; i < vertices.size(); i++) {
      g_uv_buffer_data.push_back(glm::vec2(0.0f, 0.0f));
      g_uv_buffer_data.push_back(glm::vec2(0.0f, 1.0f));
      g_uv_buffer_data.push_back(glm::vec2(1.0f, 1.0f));
    }
  } else {
    std::cout << "uvs" << std::endl;
    for (int i = 0; i < uvs.size(); i++) {
      g_uv_buffer_data.push_back(glm::vec2(uvs[i].x, uvs[i].y));
    }
  }
  GLuint uvbuffer;
  glGenBuffers(1, &uvbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
  glBufferData(GL_ARRAY_BUFFER, g_uv_buffer_data.size() * sizeof(glm::vec2),
               &g_uv_buffer_data[0], GL_STATIC_DRAW);

  // Get a handle for our "MVP" uniform
  GLuint MatrixID = glGetUniformLocation(programID, "MVP");

  // Hide the mouse and enable unlimited mouvement
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPos(window, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

  // Load it into a VBO
  GLuint vertexbuffer;
  glGenBuffers(1, &vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(ft_glm::vec3),
               &vertices[0], GL_STATIC_DRAW);

  std::vector<ft_glm::vec3> g_color_buffer_data;
  for (int i = 0; i < vertices.size(); i++) {
    g_color_buffer_data.push_back(
        ft_glm::vec3(getRandom(), getRandom(), getRandom()));
  }

  GLuint colorbuffer;
  glGenBuffers(1, &colorbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
  glBufferData(GL_ARRAY_BUFFER,
               g_color_buffer_data.size() * sizeof(ft_glm::vec3),
               &g_color_buffer_data[0], GL_STATIC_DRAW);

  double lastTime = glfwGetTime();
  double lastFrameTime = lastTime;
  int nbFrames = 0;

  float initialPositionX = (limitsX[0] + limitsX[1]) / 2.0f;
  float initialPositionZ = (limitsZ[0] + limitsZ[1]) / 2.0f;
  glm::vec3 gPosition1(initialPositionX, 0.0f, initialPositionZ);
  glm::vec3 gOrientation1(0.0f, 0.0f, 0.0f);

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
    // レンダリングここから
    // 背景色を設定
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Compute the MVP matrix from keyboard and mouse input
    computeMatricesFromInputs(window);
    ft_glm::Mat4 ProjectionMatrix = getProjectionMatrix();
    ft_glm::Mat4 ViewMatrix = getViewMatrix();
    if (getTexture()) {
      glUniform1i(useTextureLocation, 1);
    } else {
      glUniform1i(useTextureLocation, 0);
    }

    // rotation 1seconds to ModelMatrix
    // As an example, rotate arount the vertical axis at 180�/sec
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

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

    if (argc > 2) {
      // Bind our texture in Texture Unit 0
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, Texture);
      // Set our "myTextureSampler" sampler to use Texture Unit 0
      glUniform1i(TextureID, 0);
    }

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0, // attribute. No particular reason for 0, but must
                             // match the layout in the shader.
                          3, // size
                          GL_FLOAT, // type
                          GL_FALSE, // normalized?
                          0,        // stride
                          (void *)0 // array buffer offset
    );
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glVertexAttribPointer(1, // attribute. No particular reason for 1, but must
                             // match the layout in the shader.
                          3, // size
                          GL_FLOAT, // type
                          GL_FALSE, // normalized?
                          0,        // stride
                          (void *)0 // array buffer offset
    );

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(2, // attribute. No particular reason for 1, but must
                             // match the layout in the shader.
                          2, // size : U+V => 2
                          GL_FLOAT, // type
                          GL_FALSE, // normalized?
                          0,        // stride
                          (void *)0 // array buffer offset
    );

    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glfwSwapBuffers(window);
    glfwPollEvents();
  } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);

  // Cleanup VBO
  glDeleteBuffers(1, &vertexbuffer);
  glDeleteBuffers(1, &colorbuffer);
  glDeleteProgram(programID);
  glDeleteVertexArrays(1, &VertexArrayID);

  glfwTerminate();
  return 0;
}
