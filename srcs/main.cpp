#include "opengl.hpp"
#include "type.hpp"
#include <vector>

#include <cmath>

#include "glm.hpp"
#include "load.hpp"

#include <iostream>

using namespace std;

int main(const int argc, const char *argv[]) {
  GLFWwindow *window;

  // GLFWライブラリの初期化
  if (!glfwInit())
    return -1;

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  std::cout << "I'm apple machine" << std::endl;
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  // ウィンドウの作成
  window = glfwCreateWindow(1024, 768, "Hello World", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
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
    return -1;
  }

  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
  glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

  GLuint VertexArrayID;
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);

  // Read our .obj file
  std::vector<vec3> vertices;
  bool res = loadOBJ(argv[1], vertices);

  std::cout << (res ? "ok" : "no") << std::endl;

  // shader program
  GLuint programID = LoadShaders("shaders/VertexShader.vertexshader",
                                 "shaders/FragmentShader.fragmentshader");

  // Get a handle for our "MVP" uniform
  GLuint MatrixID = glGetUniformLocation(programID, "MVP");

  // camera
  Mat4 Projection = perspective(radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
  Mat4 View = lookAt(vec3(4, 3, 3), vec3(0, 0, 0), vec3(0, 1, 0));
  Mat4 Model = Mat4(1.0f);
  Mat4 MVP = Projection * View * Model;

  // Load it into a VBO
  GLuint vertexbuffer;
  glGenBuffers(1, &vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), &vertices[0],
               GL_STATIC_DRAW);

  do {
    // レンダリングここから
    // 背景色を設定
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // glClear(GL_COLOR_BUFFER_BIT);

    // Use our shader
    glUseProgram(programID);
    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

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

    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());

    glDisableVertexAttribArray(0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);

  glDeleteBuffers(1, &vertexbuffer);
  glDeleteProgram(programID);
  // glDeleteVertexArrays(1, &VertexArrayID);

  glfwTerminate();
  return 0;
}
