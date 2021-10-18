#pragma once

#include <mutex>
#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include "struct.hpp"
#include "chunkCreation.hpp"
#include "meshCreation.hpp"
#include "../glfw/include/GLFW/glfw3.h"

extern std::mutex mtx;
extern std::mutex mtxb;
extern std::mutex mtxc;
extern std::mutex mtxd;
extern std::unordered_map<glm::vec3, int, MyHashFunction> renderedKeys;

void chunkThread(ChunkMap *chunks, GLFWwindow *window, int start_x, int end_x, int start_z, int end_z);
void meshThread(ChunkMap *chunks, GLFWwindow *window, MeshMap *mesh);