#ifndef MESH_H
#define MESH_H

#include "glm/glm.hpp"
#include <vector>
#include <iostream>

float NORTH[] = {
    //NORTH
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f};
float SOUTH[] = {
    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f};
float WEST[] = {
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f};
float EAST[] = {
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, 1.0f,
    1.0f, 1.0f, 1.0f};
float BOT[] = {
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, -1.0f};
float TOP[] = {
    -1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, -1.0f};

class Mesh
{
private:
public:
    glm::mat4 mat;
    std::vector<float> tri;
    Mesh();
    ~Mesh();
    void append(float vertices)
    {
        tri.push_back(vertices);
    }
    void clearMesh()
    {
        tri.clear();
    }
    void checkIfExist(Cube cube)
    {
        std::cout << cube.Position.x << " " << cube.Position.y << " " << cube.Position.z << "\n";
        for (size_t i = 0; i < 18; i += 3)
        {

            // NORTH[i] + cube.Position.x;
            // NORTH[i + 1] + cube.Position.y;
            // NORTH[i + 2] + cube.Position.z;
        }
    }
};

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}

#endif