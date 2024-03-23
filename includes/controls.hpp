#pragma once

#include "ft_glm.hpp"
#include <GLFW/glfw3.h>

ft_glm::Mat4 getViewMatrix();
ft_glm::Mat4 getProjectionMatrix();
bool getTexture();
bool getIsRainbow();
ft_glm::vec3 getPosition();
ft_glm::vec3 getRotationAxis();

void computeMatricesFromInputs(GLFWwindow *window);