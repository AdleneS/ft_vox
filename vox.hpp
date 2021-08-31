#ifndef VOX_H
#define VOX_H
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
#include "chunk.hpp"

#include "glfw/include/GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/noise.hpp"

using namespace glm;

#define GLSL(src) #src

void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
GLuint load_texture(const char *imagePath);
void displayCube(int x, int y, int z, Shader shader, Cube ***cube);
Chunk createChunk(int chunkId, glm::vec3 offsets, int seed);
void displayChunk(Chunk *chunk, Shader shader);

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

const unsigned int CHUNK_NB = 16;
const unsigned int CHUNK_SIZE_X = 8;
const unsigned int CHUNK_SIZE_Y = 16;
const unsigned int CHUNK_SIZE_Z = 8;
#include "mesh.hpp"

#endif