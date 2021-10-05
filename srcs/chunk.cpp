#include "../headers/chunk.hpp"
#include "../headers/const.hpp"

Chunk::Chunk(glm::vec3 position, int id)
{
    Id = id;
    mat = glm::mat4(1.0f);
    Position = position;
    size = 0;
    VAO = 0;
    VBO = 0;
    UVB = 0;
    TOB = 0;
    NBO = 0;
    translate(Position);
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
void Chunk::translate(glm::vec3 v)
{
    mat = glm::translate(mat, v);
}

void Chunk::loadVBO()
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &UVB);
    glGenBuffers(1, &TOB);
    glGenBuffers(1, &NBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(float), &Vertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, UVB);
    glBufferData(GL_ARRAY_BUFFER, UV.size() * sizeof(float), &UV[0], GL_STATIC_DRAW);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, TOB);
    glBufferData(GL_ARRAY_BUFFER, texCoord.size() * sizeof(glm::vec2), &texCoord[0], GL_STATIC_DRAW);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, NBO);
    glBufferData(GL_ARRAY_BUFFER, Normal.size() * sizeof(float), &Normal[0], GL_STATIC_DRAW);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(3);
}

Chunk::~Chunk()
{
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &UVB);
    glDeleteBuffers(1, &TOB);
    glDeleteBuffers(1, &NBO);
    glDeleteVertexArrays(1, &VAO);
    Vertices.clear();
    Vertices.shrink_to_fit();
    UV.clear();
    UV.shrink_to_fit();
    texCoord.clear();
    texCoord.shrink_to_fit();
}