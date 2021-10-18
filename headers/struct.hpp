#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>
#include "chunk.hpp"
#include "mesh.hpp"
#include "vector3.hpp"
#include "buffer.hpp"

typedef std::pair<glm::vec3, Chunk *> PosChunk;
typedef std::pair<glm::vec3, Buffer *> PosBuffer;
typedef std::pair<glm::vec3, Mesh *> PosMesh;
typedef std::pair<glm::vec3, int> PosRendered;
typedef std::unordered_map<glm::vec3, Chunk *, MyHashFunction> ChunkMap;
typedef std::unordered_map<glm::vec3, Buffer *, MyHashFunction> BufferMap;
typedef std::unordered_map<glm::vec3, Mesh *, MyHashFunction> MeshMap;
typedef std::unordered_map<glm::vec3, int, MyHashFunction> KeyMap;

struct Character
{
    unsigned int TextureID;
    glm::ivec2 Size;
    glm::ivec2 Bearing;
    unsigned int Advance;
};
