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
    t_biomes biomes;
    Chunk chunk(offsets, chunkId);
    for (size_t x = 0; x < CHUNK_SIZE_X; x++)
    {
        for (size_t z = 0; z < CHUNK_SIZE_Z; z++)
        {
            int maxHeight = 0;
            biomes.height = 0;
            biomes.mountainType = mountainSimplex(x, z, seed, offsets);
            biomes.biomeType = biomeSimplex(x, z, seed, offsets);
            if (biomes.mountainType > 1.2)
                biomes.height = heightSimplex(0.5, 1.3, 0.0072, x, z, seed, offsets);
            else
                biomes.height = heightSimplex(0.5, 0.6, 0.0032, x, z, seed, offsets);

            if (biomes.height <= CHUNK_SIZE_Y / 7)
                biomes.height = CHUNK_SIZE_Y / 7;

            if (biomes.height > maxHeight)
                maxHeight = biomes.height;
            float r = noise.noise(x + offsets.x, z + offsets.z);
            r = (r + 1) / 2;
            // printf("%f\n", r);
            populateChunk(&chunk, biomes, x, z, id, maxHeight);

            if (biomes.height > CHUNK_SIZE_Y / 7 + 4 && biomes.biomeType > 0 && biomes.biomeType < 0.2)
            {
                if (r > 0.9835)
                {
                    createTree(&chunk, biomes, x, z);
                }
            }

            id = id + 1 + chunkId;

            for (int y = 0; y < (int)biomes.height; y++)
            {
                float nn = caveSimplex(x, y, z, seed, offsets);

                nn = (nn + 1) / 2;
                if (nn > 0.8)
                {
                    chunk.CubeData[x][(y)][z].Position = glm::vec3((x), (y), (z));
                    chunk.CubeData[x][(y)][z].Id = id;
                    chunk.CubeData[x][(y)][z].texCoord = glm::vec2(1, 0);
                    chunk.CubeData[x][(y)][z].isEmpty = false;
                    chunk.CubeData[x][(y)][z].value = y;
                }
                if (y == 0)
                {
                    chunk.CubeData[x][(y)][z].Position = glm::vec3((x), (y), (z));
                    chunk.CubeData[x][(y)][z].Id = id;
                    chunk.CubeData[x][(y)][z].texCoord = glm::vec2(1, 0);
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
    n *= CHUNK_SIZE_Y / 4.5;

    return n;
}

float mountainSimplex(int x, int z, float seed, glm::vec3 offsets)
{
    SimplexNoise noise;
    float n = 0.3;
    float a = 2.2;
    float freq = 0.00016;
    float value = 0;
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

float biomeSimplex(int x, int z, float seed, glm::vec3 offsets)
{
    SimplexNoise noise;
    float n = 0;
    float a = 1.0;
    float freq = 0.00016;
    float value = 0;
    for (int octave = 0; octave < 12; octave++)
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

void populateChunk(Chunk *chunk, t_biomes biomes, int x, int z, int id, int maxHeight)
{
    chunk->CubeData[x][(int)biomes.height][z].Position = glm::vec3((x), (int)(biomes.height), (z));
    chunk->CubeData[x][(int)biomes.height][z].Id = id;
    chunk->CubeData[x][(int)biomes.height][z].texCoord = selectTex(biomes);
    chunk->CubeData[x][(int)biomes.height][z].isEmpty = false;
    chunk->CubeData[x][(int)biomes.height][z].value = biomes.height;
    chunk->maxHeight = maxHeight;
}

void createTree(Chunk *chunk, t_biomes biomes, int x, int z)
{
    for (size_t i = 0; i < 5; i++)
    {
        chunk->CubeData[x][(int)biomes.height + i][z].Position = glm::vec3((x), (int)(biomes.height + i), (z));
        chunk->CubeData[x][(int)biomes.height + i][z].texCoord = glm::vec2(4, 1);
        chunk->CubeData[x][(int)biomes.height + i][z].isEmpty = false;
    }
    for (size_t j = 0; j < 3; j++)
    {
        for (size_t k = 0; k < 3; k++)
        {
            for (size_t l = 0; l < 3; l++)
            {
                if (x - 1 + k > 0 && x - 1 + k < 16 && biomes.height + 3 + l > 0 && biomes.height + 3 + l < 256 && z - 1 + j > 0 && z - 1 + j < 16)
                {
                    chunk->CubeData[x - 1 + k][(int)biomes.height + 3 + l][z - 1 + j].Position = glm::vec3((x - 1 + k), (int)(biomes.height + 3 + l), (z - 1 + j));
                    chunk->CubeData[x - 1 + k][(int)biomes.height + 3 + l][z - 1 + j].texCoord = glm::vec2(4, 8);
                    chunk->CubeData[x - 1 + k][(int)biomes.height + 3 + l][z - 1 + j].isEmpty = false;
                    chunk->maxHeight = (int)biomes.height + 3 + l;
                }
            }
        }
    }
}

glm::vec2 selectTex(t_biomes biomes)
{
    if (biomes.biomeType > 0 && biomes.biomeType < 0.2)
    {
        if ((int)biomes.height < CHUNK_SIZE_Y / 7)
            return glm::vec2(13, 12);
        if (biomes.height < CHUNK_SIZE_Y / 7 + 3)
            return glm::vec2(2, 1);
        return glm::vec2(3, 0);
    }
    if (biomes.biomeType > 0.2 && biomes.biomeType < 0.4)
    {
        if ((int)biomes.height < CHUNK_SIZE_Y / 7)
            return glm::vec2(13, 12);
        if (biomes.height < CHUNK_SIZE_Y / 7 + 3)
            return glm::vec2(2, 1);
        return glm::vec2(2, 1);
    }
    if (biomes.biomeType > 0.4 && biomes.biomeType < 0.6)
    {
        if ((int)biomes.height < CHUNK_SIZE_Y / 7)
            return glm::vec2(13, 14);
        return glm::vec2(1, 0);
    }
    if (biomes.biomeType > 0.6 && biomes.biomeType < 0.8)
    {
        if ((int)biomes.height < CHUNK_SIZE_Y / 7)
            return glm::vec2(13, 14);
        return glm::vec2(13, 4);
    }
    if ((int)biomes.height < CHUNK_SIZE_Y / 7)
        return glm::vec2(13, 12);
    if (biomes.height < CHUNK_SIZE_Y / 7 + 3)
        return glm::vec2(2, 1);
    return glm::vec2(3, 0);
}
