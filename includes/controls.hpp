#pragma once

#include "ft_glm.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

ft_glm::Mat4 getViewMatrix();
ft_glm::Mat4 getProjectionMatrix();
bool getIsLightAbove();
ft_glm::vec3 getPosition();
ft_glm::vec3 getRotationAxis();
float getMixFactor();

void computeMatricesFromInputs(GLFWwindow *window, GLuint programId);