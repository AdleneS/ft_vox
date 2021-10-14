#pragma once

#include "struct.hpp"
#include "simplex.hpp"
#include <mutex>
#include "frustum.hpp"
#include "camera.hpp"

extern std::mutex mtx;
extern std::mutex mtxb;
extern std::mutex mtxc;
extern std::mutex mtxd;
extern Frustum frustum;
extern Camera camera;

extern std::unordered_map<glm::vec3, int, MyHashFunction> renderedKeys;

float heightSimplex(float n, float a, float freq, int x, int z, float seed, glm::vec3 offsets);
float desertSimplex(int x, int z, float seed, glm::vec3 offsets);
float caveSimplex(int x, int y, int z, float seed, glm::vec3 offsets);
void populateChunk(Chunk *chunk, int x, int n, int z, int id, float b, int maxHeight);
void createTree(Chunk *chunk, int x, int n, int z, int id);
glm::vec2 selectTex(float n, float b);
int createExpendedChunkX(ChunkMap *chunks, int x, int z, int o);
Chunk createCube(int chunkId, glm::vec3 offsets, int seed);
void createChunk(ChunkMap *chunks, int start_x, int end_x, int start_z, int end_z);