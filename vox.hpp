#ifndef VOX_H
#define VOX_H
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unordered_map>
#include <string>
#include <time.h>

#include "gl3w/include/GL/gl3w.h"
#include "gl3w/src/gl3w.c"

#include "const.hpp"

#include "shader.hpp"
#include "camera.hpp"
#include "chunk.hpp"
#include "mesh.hpp"
#include "vector3.hpp"
#include "frustum.hpp"

#include "glfw/include/GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/noise.hpp"

using namespace glm;

#define GLSL(src) #src

typedef struct s_vox
{
    unsigned int chunkNbX;
    unsigned int chunkNbZ;
    unsigned int chunkCount;

    float fps;

    int seed;

} t_vox;

void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
GLuint load_texture(const char *imagePath);

Chunk createCube(t_vox *vox, int chunkId, glm::vec3 offsets, int seed);
void createChunk(t_vox *vox, std::unordered_map<vec3, Chunk *, MyHashFunction> *chunks);
void createMesh(Chunk *chunk);
void displayChunk(Shader shader, t_vox *vox, std::unordered_map<vec3, Chunk *, MyHashFunction> *chunks, Frustum frustum);

float Get2DPerlinNoiseValue(float x, float y, float res);

t_vox *init();

#endif