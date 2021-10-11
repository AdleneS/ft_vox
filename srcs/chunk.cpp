#include "../headers/chunk.hpp"

Chunk::Chunk(glm::vec3 position, int id)
{
    Id = id;
    Position = position;
    rendered = false;
    initCubeData();
    memCubeData();
}
void Chunk::initCubeData()
{
    CubeData = (Cube ***)malloc(CHUNK_SIZE_X * sizeof(Cube **));

    for (int i = 0; i < CHUNK_SIZE_X; i++)
    {
        CubeData[i] = (Cube **)malloc(CHUNK_SIZE_Y * sizeof(Cube *));
        for (int j = 0; j < CHUNK_SIZE_Y; j++)
        {
            CubeData[i][j] = (Cube *)malloc(CHUNK_SIZE_Z * sizeof(Cube));
        }
    }
}
void Chunk::memCubeData()
{
    for (size_t x = 0; x < CHUNK_SIZE_X; x++)
    {
        for (size_t y = 0; y < CHUNK_SIZE_Y; y++)
        {
            for (size_t z = 0; z < CHUNK_SIZE_Z; z++)
            {
                CubeData[x][y][z].isEmpty = true;
            }
        }
    }
}
void Chunk::freeCubeData()
{
    for (int i = 0; i < CHUNK_SIZE_X; i++)
    {
        for (int j = 0; j < CHUNK_SIZE_Y; j++)
        {
            free(CubeData[i][j]);
        }
        free(CubeData[i]);
    }
    free(CubeData);
}

Chunk::~Chunk()
{
    // printf("DELETE\n");
    freeCubeData();
}