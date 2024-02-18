#pragma once

#include "ft_glm.hpp"
#include <GLFW/glfw3.h>

extern GLFWwindow *window;

ft_glm::Mat4 getViewMatrix();
ft_glm::Mat4 getProjectionMatrix();

void computeMatricesFromInputs();