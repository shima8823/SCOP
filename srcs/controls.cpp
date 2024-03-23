#include "controls.hpp"

#include "const.hpp"
#include <iostream>

using namespace ft_glm;

Mat4 ViewMatrix;
Mat4 ProjectionMatrix;
bool texture = false;
vec3 position = vec3(0, 4, 13);
float horizontalAngle = M_PI;
float verticalAngle = 0.0f;
float initialFoV = 45.0f;
float speed = 3.0f;
float mouseSpeed = 0.005f;

Mat4 getViewMatrix() { return ViewMatrix; }
Mat4 getProjectionMatrix() { return ProjectionMatrix; }
bool getTexture() { return texture; }
vec3 getPosition() { return position; }

void computeMatricesFromInputs(GLFWwindow *window) {

  static double lastTime = glfwGetTime();
  static double lastTimeKeyTPressed = 0.0;

  double currentTime = glfwGetTime();
  float deltaTime = float(currentTime - lastTime);

  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);
  glfwSetCursorPos(window, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

  horizontalAngle += mouseSpeed * float(SCREEN_WIDTH / 2 - xpos);
  verticalAngle += mouseSpeed * float(SCREEN_HEIGHT / 2 - ypos);

  vec3 direction(cos(verticalAngle) * sin(horizontalAngle), sin(verticalAngle),
                 cos(verticalAngle) * cos(horizontalAngle));
  vec3 right = vec3(sin(horizontalAngle - M_PI / 2.0f), 0,
                    cos(horizontalAngle - M_PI / 2.0f));
  vec3 up = cross(right, direction);

  // keydown shift + up
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS &&
      glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
    position += vec3(0, 1, 0) * deltaTime * speed;
  }

  // keydown shift + down
  else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS &&
           glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    position -= vec3(0, 1, 0) * deltaTime * speed;
  }

  // keydown up
  else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
    position += direction * deltaTime * speed;
  }

  // keydown down
  else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    position -= direction * deltaTime * speed;
  }

  // keydown right
  else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
    position += right * deltaTime * speed;
  }

  // keydown left
  else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
    position -= right * deltaTime * speed;
  }

  // keydown t
  else if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
    if (currentTime - lastTimeKeyTPressed >= 1.0) { // cooldown 1s
      texture = !texture;
      lastTimeKeyTPressed = currentTime;
    }
  }

  float FoV = initialFoV;
  ProjectionMatrix = perspective(radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
  ViewMatrix = lookAt(position, position + direction, up);
  lastTime = currentTime;
}