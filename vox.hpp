#include <stdio.h>
#include <stdlib.h>
#include <iostream>

//#define GLFW_DLL
//#include <GL/glew.h>
#include "gl3w/include/GL/gl3w.h"
#include "gl3w/src/gl3w.c"

#include "shader.hpp"
#include "camera.hpp"
#include "cube.hpp"
#include "mesh.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.hpp"

#include "glfw/include/GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/gtc/type_ptr.hpp"

using namespace glm;

#define GLSL(src) #src

void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
GLuint load_texture(const char *imagePath);
void displayCube(Cube cube, Shader shader, int skip = -1);
void createChunk(Shader shader);

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;