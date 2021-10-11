#ifndef VOX_H
#define VOX_H
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unordered_map>
#include <string>
#include <time.h>
#include <algorithm>
#include <thread>
#include <mutex>
#include "../gl3w/include/GL/gl3w.h"
#include "../gl3w/src/gl3w.c"

#include "const.hpp"

#include "shader.hpp"
#include "camera.hpp"
#include "chunk.hpp"
#include "mesh.hpp"
#include "vector3.hpp"
#include "frustum.hpp"
#include "simplex.hpp"
#include "cubemap.hpp"
#include "buffer.hpp"

#include "../glfw/include/GLFW/glfw3.h"
#include "../glm/glm.hpp"
#include "../glm/ext/matrix_clip_space.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include "../glm/gtc/noise.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.hpp"

#define GLSL(src) #src

typedef std::pair<glm::vec3, Chunk *> PosChunk;
typedef std::pair<glm::vec3, Buffer *> PosBuffer;
typedef std::pair<glm::vec3, Mesh *> PosMesh;
typedef std::pair<glm::vec3, unsigned int> PosRendered;
typedef std::unordered_map<glm::vec3, Chunk *, MyHashFunction> ChunkMap;
typedef std::unordered_map<glm::vec3, Buffer *, MyHashFunction> BufferMap;
typedef std::unordered_map<glm::vec3, Mesh *, MyHashFunction> MeshMap;
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
void createChunk(t_vox *vox, std::unordered_map<glm::vec3, Chunk *, MyHashFunction> *chunks, int start_x, int end_x, int start_z, int end_z);
Mesh createMesh(Chunk chunk);
void displayChunk(BufferMap *buffer, MeshMap *mesh, Shader shader);
void displayVAO(BufferMap *buffer, Shader shader);

glm::vec2 selectTex(float n, float b);

float Get2DPerlinNoiseValue(float x, float y, float res);

t_vox *init();

#endif