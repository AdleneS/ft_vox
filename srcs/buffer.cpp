#include "../headers/buffer.hpp"

Buffer::Buffer()
{
    VAO = 0;
    VBO = 0;
    UVB = 0;
    TOB = 0;
    NBO = 0;
}

Buffer::~Buffer()
{
    deleteVBO();
}

void Buffer::deleteVBO()
{
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &UVB);
    glDeleteBuffers(1, &TOB);
    glDeleteBuffers(1, &NBO);
    glDeleteVertexArrays(1, &VAO);
}
void Buffer::loadVBO(Mesh *mesh)
{
    //printf("%d\n", chunk.Vertices.size());
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &UVB);
    glGenBuffers(1, &TOB);
    glGenBuffers(1, &NBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh->Vertices.size() * sizeof(glm::vec3), &mesh->Vertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, UVB);
    glBufferData(GL_ARRAY_BUFFER, mesh->UV.size() * sizeof(glm::vec2), &mesh->UV[0], GL_STATIC_DRAW);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, TOB);
    glBufferData(GL_ARRAY_BUFFER, mesh->texCoord.size() * sizeof(glm::vec2), &mesh->texCoord[0], GL_STATIC_DRAW);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, NBO);
    glBufferData(GL_ARRAY_BUFFER, mesh->Normal.size() * sizeof(glm::vec3), &mesh->Normal[0], GL_STATIC_DRAW);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(3);
}
