#pragma once

#include <mutex>
#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include "struct.hpp"
#include "chunkCreation.hpp"
#include "meshCreation.hpp"

extern std::mutex mtx;
extern std::mutex mtxb;
extern std::mutex mtxc;
extern std::mutex mtxd;
extern std::unordered_map<glm::vec3, int, MyHashFunction> renderedKeys;

void chunkThread(ChunkMap *chunks, int start_x, int end_x, int start_z, int end_z);
void meshThread(ChunkMap *chunks, MeshMap *mesh);