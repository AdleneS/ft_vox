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
#include "cubemap.hpp"
#include "buffer.hpp"
#include "utils.hpp"
#include "threadFunction.hpp"

#include "../glfw/include/GLFW/glfw3.h"
#include "../glm/glm.hpp"
#include "../glm/ext/matrix_clip_space.hpp"
#include "../glm/gtc/type_ptr.hpp"

#define GLSL(src) #src

#include "struct.hpp"

void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

void displayVAO(BufferMap *buffer, Shader shader);
void skyboxDisplay(Shader skyboxShader, Cubemap skybox, glm::mat4 projection);

Camera camera(glm::vec3(0.0f, 128.0f, 0.0f));
Frustum frustum;
std::unordered_map<glm::vec3, int, MyHashFunction> renderedKeys;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

std::mutex mtx;
std::mutex mtxb;
std::mutex mtxc;
std::mutex mtxd;

#endif