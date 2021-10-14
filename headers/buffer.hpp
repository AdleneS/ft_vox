#pragma once

#include "../gl3w/include/GL/gl3w.h"
#include "../glm/glm.hpp"
#include "mesh.hpp"
#include <cstdio>
class Buffer
{
private:
    /* data */
public:
    GLuint VAO;
    GLuint VBO;
    GLuint UVB;
    GLuint TOB;
    GLuint NBO;
    glm::vec3 Position;
    glm::mat4x4 mat;
    int size;
    int maxHeight;
    Buffer();
    ~Buffer();
    void loadVBO(Mesh *mesh);
    void deleteVBO();
};