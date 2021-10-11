#ifndef MESH_H
#define MESH_H

#include "../glm/glm.hpp"
#include "cube.hpp"
#include "const.hpp"
#include <vector>
#include <iostream>

enum class Direction
{
    North,
    East,
    South,
    West,
    Top,
    Bot
};

struct CubeCoordinate
{
    int x;
    int y;
    int z;
};

const CubeCoordinate offsets[] =
    {
        {0, 0, 1},
        {1, 0, 0},
        {0, 0, -1},
        {-1, 0, 0},
        {0, 1, 0},
        {0, -1, 0}};

class Mesh
{
private:
public:
    std::vector<glm::vec3> Vertices;
    std::vector<glm::vec3> Normal;
    std::vector<glm::vec2> texCoord;
    std::vector<glm::vec2> UV;
    glm::vec3 Position;
    glm::mat4x4 mat;
    int maxHeight;
    int size;
    bool rendered;
    Mesh(glm::vec3 position);
    //~Mesh();
    void translate(glm::vec3 v);
    int getCube(Cube neighbor, Cube current, int dir);
    int getNeighbor(int x, int y, int z, Direction dir, Cube ***cube);
};

#endif