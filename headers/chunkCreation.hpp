#pragma once

#include <mutex>
#include "struct.hpp"
#include "simplex.hpp"
#include "frustum.hpp"
#include "camera.hpp"

extern std::mutex mtx;
extern std::mutex mtxb;
extern std::mutex mtxc;
extern std::mutex mtxd;
extern Frustum frustum;
extern Camera camera;

extern std::unordered_map<glm::vec3, int, MyHashFunction> renderedKeys;

typedef struct s_biomes
{
    float mountainType;
    float biomeType;
    float height;

} t_biomes;

glm::vec2 selectTex(t_biomes biomes);
Chunk createCube(int chunkId, glm::vec3 offsets, int seed);
float caveSimplex(int x, int y, int z, float seed, glm::vec3 offsets);
float mountainSimplex(int x, int z, float seed, glm::vec3 offsets);
float heightSimplex(float n, float a, float freq, int x, int z, float seed, glm::vec3 offsets);
float biomeSimplex(int x, int z, float seed, glm::vec3 offsets);
void createTree(Chunk *chunk, t_biomes biomes, int x, int z);
void populateChunk(Chunk *chunk, t_biomes biomes, int x, int z, int id, int maxHeight);
void createChunk(ChunkMap *chunks, int start_x, int end_x, int start_z, int end_z);
int createExpendedChunkX(ChunkMap *chunks, int x, int z, int o);