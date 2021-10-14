#include "../headers/chunkCreation.hpp"

void createChunk(ChunkMap *chunks, int start_x, int end_x, int start_z, int end_z)
{
    int new_view_distance_x = ((end_x) + (int)((int)camera.Position.x / CHUNK_SIZE_X));
    int new_view_distance_z = ((end_z) + (int)((int)camera.Position.z / CHUNK_SIZE_Z));
    size_t o = 0;

    for (int x = (start_x) + (int)((int)camera.Position.x / CHUNK_SIZE_X); x < new_view_distance_x; x++)
    {
        for (int z = (start_z) + (int)((int)camera.Position.z / CHUNK_SIZE_Z); z < new_view_distance_z; z++)
        {
            Frustum::Visibility visiblity = frustum.IsInside(glm::vec3(x * CHUNK_SIZE_X, 256 / 2, z * CHUNK_SIZE_Z));
            if (visiblity == Frustum::Partially || visiblity == Frustum::Completly)
            {
                if (createExpendedChunkX(chunks, x, z, o))
                    return;
            }
            o++;
        }
    }
}

int createExpendedChunkX(ChunkMap *chunks, int x, int z, int o)
{
    mtxd.lock();
    mtx.lock();
    if (renderedKeys.find(glm::vec3(x, 0, z)) == renderedKeys.end() && chunks->find(glm::vec3(x, 0, z)) == chunks->end() && chunks->size() < 50)
    {
        renderedKeys.emplace(PosRendered(glm::vec3(x, 0, z), 0));
        mtxd.unlock();
        chunks->emplace(PosChunk(glm::vec3(x, 0, z), new Chunk(createCube(o, glm::vec3(x * CHUNK_SIZE_X, 0, z * CHUNK_SIZE_Z), 0))));
        mtx.unlock();
        return 1;
    }
    mtx.unlock();
    mtxd.unlock();
    return 0;
}

Chunk createCube(int chunkId, glm::vec3 offsets, int seed)
{
    int id = 0;
    SimplexNoise noise;
    Chunk chunk(offsets, chunkId);
    for (size_t x = 0; x < CHUNK_SIZE_X; x++)
    {
        for (size_t z = 0; z < CHUNK_SIZE_Z; z++)
        {
            float n = 0;
            int maxHeight = 0;
            float b = desertSimplex(x, z, seed, offsets);
            if (b > 1.2)
                n = heightSimplex(0.5, 0.3, 0.0072, x, z, seed, offsets);
            else
                n = heightSimplex(0.5, 0.3, 0.0032, x, z, seed, offsets);

            if (n <= CHUNK_SIZE_Y / 3)
                n = CHUNK_SIZE_Y / 3;

            if (n > maxHeight)
                maxHeight = n;
            float r = noise.noise(x + offsets.x, z + offsets.z);
            r = (r + 1) / 2;
            // printf("%f\n", r);
            populateChunk(&chunk, x, n, z, id, b, maxHeight);

            if (n > CHUNK_SIZE_Y / 3 + 4 && b < 0.9)
            {
                if (r > 0.9835)
                {
                    createTree(&chunk, x, n, z, id);
                }
            }

            id = id + 1 + chunkId;

            for (int y = 50; y < (int)n; y++)
            {
                float nn = caveSimplex(x, y, z, seed, offsets);

                nn = (nn + 1) / 2;
                if (nn > 0.8)
                {
                    chunk.CubeData[x][(y)][z].Position = glm::vec3((x), (y), (z));
                    chunk.CubeData[x][(y)][z].Id = id;
                    chunk.CubeData[x][(y)][z].texCoord = glm::vec2(0, 2);
                    chunk.CubeData[x][(y)][z].isEmpty = false;
                    chunk.CubeData[x][(y)][z].value = y;
                }
                if (y == 50)
                {
                    chunk.CubeData[x][(y)][z].Position = glm::vec3((x), (y), (z));
                    chunk.CubeData[x][(y)][z].Id = id;
                    chunk.CubeData[x][(y)][z].texCoord = glm::vec2(0, 2);
                    chunk.CubeData[x][(y)][z].isEmpty = false;
                    chunk.CubeData[x][(y)][z].value = y;
                }
            }
        }
    }
    return chunk;
}

float heightSimplex(float n, float a, float freq, int x, int z, float seed, glm::vec3 offsets)
{
    SimplexNoise noise;
    // float n = 0.5;
    // float a = 0.3;
    // float freq = 0.0032;
    float value = 0;
    for (int octave = 0; octave < 8; octave++)
    {
        value = a * noise.noise((x + offsets.x + (float)seed) * freq, (z + offsets.z + (float)seed) * freq, 1);
        n += value;
        a *= 0.5;
        freq *= 2.0;
    }
    n = (n + 1) / 2;
    n *= CHUNK_SIZE_Y / 2;

    return n;
}

float desertSimplex(int x, int z, float seed, glm::vec3 offsets)
{
    SimplexNoise noise;
    float n = 0.3;
    float a = 2.2;
    float freq = 0.0008;
    float value = 3;
    for (int octave = 0; octave < 8; octave++)
    {
        value = a * noise.noise((x + offsets.x + (float)seed) * freq, (z + offsets.z + (float)seed) * freq, 1);
        n += value;
        a *= 0.5;
        freq *= 2.0;
    }
    n = (n + 1) / 2;

    return n;
}

float caveSimplex(int x, int y, int z, float seed, glm::vec3 offsets)
{
    SimplexNoise noise;
    float n = 0.5;
    float a = 0.7;
    float freq = 0.0632;
    float value = 0;
    for (int octave = 0; octave < 2; octave++)
    {
        value = a * noise.noise((x + offsets.x + (float)seed) * freq, (y + (float)seed) * freq, (z + offsets.z + (float)seed) * freq);
        n += value;
        a *= 0.5;
        freq *= 2.0;
    }
    return n;
}

void populateChunk(Chunk *chunk, int x, int n, int z, int id, float b, int maxHeight)
{
    chunk->CubeData[x][(int)n][z].Position = glm::vec3((x), (int)(n), (z));
    chunk->CubeData[x][(int)n][z].Id = id;
    chunk->CubeData[x][(int)n][z].texCoord = selectTex(n, b);
    chunk->CubeData[x][(int)n][z].isEmpty = false;
    chunk->CubeData[x][(int)n][z].value = n;
    chunk->maxHeight = maxHeight;
}

void createTree(Chunk *chunk, int x, int n, int z, int id)
{
    for (size_t i = 0; i < 5; i++)
    {
        chunk->CubeData[x][(int)n + i][z].Position = glm::vec3((x), (int)(n + i), (z));
        chunk->CubeData[x][(int)n + i][z].texCoord = glm::vec2(0, 3);
        chunk->CubeData[x][(int)n + i][z].isEmpty = false;
    }
    for (size_t j = 0; j < 3; j++)
    {
        for (size_t k = 0; k < 3; k++)
        {
            for (size_t l = 0; l < 3; l++)
            {
                if (x - 1 + k > 0 && x - 1 + k < 16 && n + 3 + l > 0 && n + 3 + l < 256 && z - 1 + j > 0 && z - 1 + j < 16)
                {
                    chunk->CubeData[x - 1 + k][(int)n + 3 + l][z - 1 + j].Position = glm::vec3((x - 1 + k), (int)(n + 3 + l), (z - 1 + j));
                    chunk->CubeData[x - 1 + k][(int)n + 3 + l][z - 1 + j].texCoord = glm::vec2(2, 3);
                    chunk->CubeData[x - 1 + k][(int)n + 3 + l][z - 1 + j].isEmpty = false;
                    chunk->maxHeight = (int)n + 3 + l;
                }
            }
        }
    }
}

glm::vec2 selectTex(float n, float b)
{
    if (n < CHUNK_SIZE_Y / 3)
        return glm::vec2(0, 1);
    if (n < CHUNK_SIZE_Y / 3 + 3)
        return glm::vec2(1, 1);
    if (b > 0.9 && b < 1.2)
        return glm::vec2(1, 1);
    if (b > 1.2)
        return glm::vec2(0, 2);
    return glm::vec2(0, 0);
}
