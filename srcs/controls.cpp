#include "controls.hpp"

#include "const.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

using namespace ft_glm;

Mat4 ViewMatrix;
Mat4 ProjectionMatrix;
bool texture = false;
Mat4 getViewMatrix() { return ViewMatrix; }
Mat4 getProjectionMatrix() { return ProjectionMatrix; }
bool getTexture() { return texture; }

// Initial position : on +Z
vec3 position = vec3(0, 4, 13);
// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;

float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.005f;

void computeMatricesFromInputs(GLFWwindow *window) {

  // glfwGetTime is called only once, the first time this function is called
  static double lastTime = glfwGetTime();
  static double lastTimeKeyTPressed =
      0.0; // Initialize with 0 to allow immediate first use

  // Compute time difference between current and last frame
  double currentTime = glfwGetTime();
  float deltaTime = float(currentTime - lastTime);

  // Get mouse position
  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);
  if (xpos < 0 || SCREEN_WIDTH < xpos || ypos < 0 || SCREEN_HEIGHT < ypos ||
      xpos < ypos) {
    xpos = SCREEN_WIDTH / 2;
    ypos = SCREEN_HEIGHT / 2;
  }

  //   std::cout << "xpos: " << xpos << " ypos: " << ypos << std::endl;

  // Reset mouse position for next frame
  glfwSetCursorPos(window, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

  // Compute new orientation
  horizontalAngle += mouseSpeed * float(SCREEN_WIDTH / 2 - xpos);
  verticalAngle += mouseSpeed * float(SCREEN_HEIGHT / 2 - ypos);

  // Direction : Spherical coordinates to Cartesian coordinates conversion
  vec3 direction(cos(verticalAngle) * sin(horizontalAngle), sin(verticalAngle),
                 cos(verticalAngle) * cos(horizontalAngle));

  // Right vector
  vec3 right = vec3(sin(horizontalAngle - 3.14f / 2.0f), 0,
                    cos(horizontalAngle - 3.14f / 2.0f));

  // Up vector
  vec3 up = right.cross(direction);

  // Move forward
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
    position += direction * deltaTime * speed;
  }
  // Move backward
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    position -= direction * deltaTime * speed;
  }
  // Strafe right
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
    position += right * deltaTime * speed;
  }
  // Strafe left
  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
    position -= right * deltaTime * speed;
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS &&
      glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
    // world position y is up
    position += up * deltaTime * speed;
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS &&
      glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    position -= up * deltaTime * speed;
  }

  // keydown t
  //  押されたら1秒間クールダウンを設定
  if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
    if (currentTime - lastTimeKeyTPressed >= 1.0) {
      texture = !texture;
      lastTimeKeyTPressed =
          currentTime; // Update the last pressed time for cooldown
    }
  }

  float FoV =
      initialFoV; // - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting
                  // up a callback for this. It's a bit too complicated for this
                  // beginner's tutorial, so it's disabled instead.

  // Projection matrix : 45 Field of View, 4:3 ratio, display range : 0.1 unit
  // <-> 100 units
  ProjectionMatrix = perspective(radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
  // Camera matrix
  ViewMatrix = lookAt(
      position, // Camera is here
      position +
          direction, // and looks here : at the same position, plus "direction"
      up             // Head is up (set to 0,-1,0 to look upside-down)
  );

  // For the next frame, the "last time" will be "now"
  lastTime = currentTime;
}