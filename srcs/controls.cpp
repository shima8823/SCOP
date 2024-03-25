#include <iostream>

#include "const.hpp"
#include "controls.hpp"

using namespace ft_glm;

Mat4 ViewMatrix;
Mat4 ProjectionMatrix;
bool texture = false;
bool isRainbow = false;
bool isLightAbove = true;
bool isFirst = true;
bool isSecond = true;
vec3 position = vec3(0, 4, 13);
vec3 rotationAxis = vec3(0, 1, 0);
float horizontalAngle = M_PI;
float verticalAngle = 0.0f;
float initialFoV = 45.0f;
float speed = 3.0f;
float mouseSpeed = 0.005f;

Mat4 getViewMatrix() { return ViewMatrix; }
Mat4 getProjectionMatrix() { return ProjectionMatrix; }
bool getTexture() { return texture; }
bool getIsRainbow() { return isRainbow; }
bool getIsLightAbove() { return isLightAbove; }
vec3 getPosition() { return position; }
vec3 getRotationAxis() { return rotationAxis; }

void pointCameraToOrigin() {
  vec3 directionToObject = normalize(ft_glm::vec3(0, 0, 0) - position);
  horizontalAngle = atan2(directionToObject.x, directionToObject.z);
  verticalAngle = asin(directionToObject.y);
}

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

  // xpos, ypos wrong value in 2frame
  if (isFirst) {
    pointCameraToOrigin();
    isFirst = false;
  } else if (isSecond) {
    pointCameraToOrigin();
    isSecond = false;
  }

  vec3 direction(cos(verticalAngle) * sin(horizontalAngle), sin(verticalAngle),
                 cos(verticalAngle) * cos(horizontalAngle));
  vec3 right = vec3(sin(horizontalAngle - M_PI / 2.0f), 0,
                    cos(horizontalAngle - M_PI / 2.0f));
  vec3 up = cross(right, direction);

  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
    // keydown up
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
      position += vec3(0, 1, 0) * deltaTime * speed;
    //   keydown down
    else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
      position -= vec3(0, 1, 0) * deltaTime * speed;
    // keydown t
    else if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
      if (currentTime - lastTimeKeyTPressed >= 1.0) { // cooldown 1s
        texture = !texture;
        lastTimeKeyTPressed = currentTime;
      }
    }
    // keydown x
    else if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
      if (currentTime - lastTimeKeyTPressed >= 1.0) {
        rotationAxis = vec3(1, 0, 0);
        lastTimeKeyTPressed = currentTime;
      }
    }
    // keydown y
    else if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
      if (currentTime - lastTimeKeyTPressed >= 1.0) {
        rotationAxis = vec3(0, 1, 0);
        lastTimeKeyTPressed = currentTime;
      }
    }
    // keydown z
    else if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
      if (currentTime - lastTimeKeyTPressed >= 1.0) {
        rotationAxis = vec3(0, 0, 1);
        lastTimeKeyTPressed = currentTime;
      }
    }
    // keydown z
    else if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
      if (currentTime - lastTimeKeyTPressed >= 1.0) {
        rotationAxis = vec3(0, 0, 1);
        lastTimeKeyTPressed = currentTime;
      }
    }
    // keydown f
    else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
      if (currentTime - lastTimeKeyTPressed >= 1.0) {
        isRainbow = !isRainbow;
        lastTimeKeyTPressed = currentTime;
      }
    }
    // keydown p
    else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
      if (currentTime - lastTimeKeyTPressed >= 1.0) {
        pointCameraToOrigin();
        lastTimeKeyTPressed = currentTime;
      }
    }
    // keydown l
    else if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
      if (currentTime - lastTimeKeyTPressed >= 1.0) {
        isLightAbove = !isLightAbove;
        lastTimeKeyTPressed = currentTime;
      }
    }
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

  float FoV = initialFoV;
  ProjectionMatrix = perspective(radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
  ViewMatrix = lookAt(position, position + direction, up);
  lastTime = currentTime;
}