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
bool startAnimation = false;
vec3 position = vec3(0, 4, 13);
vec3 rotationAxis = vec3(0, 1, 0);
float horizontalAngle = M_PI;
float verticalAngle = 0.0f;
float initialFoV = 45.0f;
float speed = 3.0f;
float mouseSpeed = 0.005f;
float mixFactor = 0.0f;
float animationDuration = 1.0f;
int prevColor = 0; // 0: white, 1: rainbow, 2: texture
double animationStartTime = 0.0;

Mat4 getViewMatrix() { return ViewMatrix; }
Mat4 getProjectionMatrix() { return ProjectionMatrix; }
bool getTexture() { return texture; }
bool getIsRainbow() { return isRainbow; }
bool getIsLightAbove() { return isLightAbove; }
vec3 getPosition() { return position; }
vec3 getRotationAxis() { return rotationAxis; }
float getMixFactor() { return mixFactor; }

void pointCameraToOrigin() {
  vec3 directionToObject = normalize(ft_glm::vec3(0, 0, 0) - position);
  horizontalAngle = atan2(directionToObject.x, directionToObject.z);
  verticalAngle = asin(directionToObject.y);
}

bool isCooldownFinished(double currentTime, double lastTimeKeyTPressed) {
  return currentTime - lastTimeKeyTPressed >= 1.0;
}

void computeMatricesFromInputs(GLFWwindow *window, GLuint programId) {

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
      if (isCooldownFinished(currentTime, lastTimeKeyTPressed) &&
          !startAnimation) { // cooldown 1s
        if (isRainbow) {
          prevColor = 1;
          isRainbow = false;
        } else if (texture) {
          prevColor = 2;
        }
        texture = !texture;
        startAnimation = true;
        animationStartTime = glfwGetTime();
        lastTimeKeyTPressed = currentTime;
      }
    }
    // keydown x
    else if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
      if (isCooldownFinished(currentTime, lastTimeKeyTPressed)) {
        rotationAxis = vec3(1, 0, 0);
        lastTimeKeyTPressed = currentTime;
      }
    }
    // keydown y
    else if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
      if (isCooldownFinished(currentTime, lastTimeKeyTPressed)) {
        rotationAxis = vec3(0, 1, 0);
        lastTimeKeyTPressed = currentTime;
      }
    }
    // keydown z
    else if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
      if (isCooldownFinished(currentTime, lastTimeKeyTPressed)) {
        rotationAxis = vec3(0, 0, 1);
        lastTimeKeyTPressed = currentTime;
      }
    }
    // keydown z
    else if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
      if (isCooldownFinished(currentTime, lastTimeKeyTPressed)) {
        rotationAxis = vec3(0, 0, 1);
        lastTimeKeyTPressed = currentTime;
      }
    }
    // keydown f
    else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
      if (isCooldownFinished(currentTime, lastTimeKeyTPressed) &&
          !startAnimation) {
        if (texture) {
          prevColor = 2;
          texture = false;
        } else if (isRainbow) {
          prevColor = 1;
        }
        isRainbow = !isRainbow;
        startAnimation = true;
        animationStartTime = glfwGetTime();
        lastTimeKeyTPressed = currentTime;
      }
    }
    // keydown p
    else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
      if (isCooldownFinished(currentTime, lastTimeKeyTPressed)) {
        pointCameraToOrigin();
        lastTimeKeyTPressed = currentTime;
      }
    }
    // keydown l
    else if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
      if (isCooldownFinished(currentTime, lastTimeKeyTPressed)) {
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

  if (startAnimation) {
    float elapsedTime = (float)(currentTime - animationStartTime);
    mixFactor = elapsedTime / animationDuration;

    if (mixFactor > 1.0f) {
      mixFactor = 1.0f;
      if (!isRainbow || !texture)
        prevColor = 0;
      startAnimation = false;
    }
  }

  glUniform1f(glGetUniformLocation(programId, "mixFactor"), mixFactor);
  glUniform1i(glGetUniformLocation(programId, "prevColor"), prevColor);

  float FoV = initialFoV;
  ProjectionMatrix = perspective(radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
  ViewMatrix = lookAt(position, position + direction, up);
  lastTime = currentTime;
}