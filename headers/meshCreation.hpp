#pragma once

#include "struct.hpp"
#include "shader.hpp"

Mesh createMesh(Chunk chunk);
void bufferGeneration(BufferMap *buffer, MeshMap *mesh);