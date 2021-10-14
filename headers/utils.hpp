#pragma once

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include "stb_image.hpp"
#include <iostream>
#include "../gl3w/include/GL/gl3w.h"
#include "../glfw/include/GLFW/glfw3.h"

GLuint load_texture(const char *imagePath);
void fps(GLFWwindow *window);