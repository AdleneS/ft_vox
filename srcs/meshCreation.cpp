#include "../headers/meshCreation.hpp"

Mesh createMesh(Chunk chunk)
{
    Mesh mesh(chunk.Position);
    glm::vec2 tex = glm::vec2(0, 0);
    for (size_t x = 0; x < CHUNK_SIZE_X; x++)
    {
        for (size_t y = 0; y < CHUNK_SIZE_Y; y++)
        {
            for (size_t z = 0; z < CHUNK_SIZE_Z; z++)
            {
                if (chunk.CubeData[x][y][z].isEmpty)
                    continue;
                for (int i = 0; i < 6; i++)
                {
                    tex = chunk.CubeData[x][y][z].texCoord;
                    if (glm::vec2(3, 0) == tex && i == 4)
                        tex = glm::vec2(8, 2);
                    if ((glm::vec2(3, 0) == tex || glm::vec2(13, 4) == tex) && i == 5)
                        tex = glm::vec2(2, 0);
                    if (glm::vec2(13, 4) == tex && i == 4)
                        tex = glm::vec2(14, 4);
                    if (!mesh.getNeighbor(x, y, z, (Direction)i, chunk.CubeData) || chunk.CubeData[x][y][z].texCoord == glm::vec2(4, 8))
                    {
                        for (size_t j = 0; j < 18; j += 3)
                        {
                            mesh.Vertices.push_back(glm::vec3(VERTICES[j + (i * 18)] + chunk.CubeData[x][y][z].Position.x,
                                                              VERTICES[j + 1 + (i * 18)] + chunk.CubeData[x][y][z].Position.y,
                                                              VERTICES[j + 2 + (i * 18)] + chunk.CubeData[x][y][z].Position.z));

                            mesh.Normal.push_back(glm::vec3(NORMAL[j + (i * 18)],
                                                            NORMAL[j + 1 + (i * 18)],
                                                            NORMAL[j + 2 + (i * 18)]));
                            mesh.texCoord.push_back(tex);
                            mesh.size += 3;
                        }
                        for (size_t k = 0; k < 12; k += 2)
                        {
                            mesh.UV.push_back(glm::vec2(UV[k + (i * 12)],
                                                        UV[k + 1 + (i * 12)]));
                        }
                    }
                }
            }
        }
    }
    mesh.maxHeight = chunk.maxHeight;
    return mesh;
}

void bufferGeneration(BufferMap *buffer, MeshMap *mesh, Shader shader)
{
    auto begin = mesh->begin();
    auto end = mesh->end();
    // printf("%d\n", mesh->size());
    if (mesh->size() > 0)
    {
        if (begin != end)
        {
            buffer->emplace(PosBuffer(begin->first, new Buffer()));
            buffer->at(begin->first)->mat = begin->second->mat;
            buffer->at(begin->first)->maxHeight = begin->second->maxHeight;
            buffer->at(begin->first)->size = begin->second->size;
            buffer->at(begin->first)->Position = begin->second->Position;
            buffer->at(begin->first)->loadVBO(begin->second);
            mesh->at(begin->first)->Vertices.clear();
            mesh->at(begin->first)->Vertices.shrink_to_fit();
            mesh->at(begin->first)->UV.clear();
            mesh->at(begin->first)->UV.shrink_to_fit();
            mesh->at(begin->first)->texCoord.clear();
            mesh->at(begin->first)->texCoord.shrink_to_fit();
            mesh->at(begin->first)->Normal.clear();
            mesh->at(begin->first)->Normal.shrink_to_fit();
            mesh->erase(begin);
            return;
        }
    }
}