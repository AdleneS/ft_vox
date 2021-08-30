#ifndef MESH_H
#define MESH_H

#include "glm/glm.hpp"
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

CubeCoordinate offsets[] =
    {
        0, 0, 1,
        1, 0, 0,
        0, 0, -1,
        -1, 0, 0,
        0, 1, 0,
        0, -1, 0};

class Mesh
{
private:
public:
    glm::vec3 Position;
    Mesh(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f))
    {
        Position = position;
        //translate(Position);
    }
    int getCube(int x, int y, int z, Cube ***cube)
    {
        if (cube[x][y][z].Id >= 0 && !cube[x][y][z].isEmpty)
            return 1;
        else
            return 0;
    }
    int getNeighbor(int x, int y, int z, Direction dir, Cube ***cube)
    {
        CubeCoordinate offsetToCheck = offsets[(int)dir];
        CubeCoordinate neighborCoord = {x + offsetToCheck.x, y + offsetToCheck.y, z + offsetToCheck.z};

        if (neighborCoord.x < 0 || neighborCoord.x >= 16 || neighborCoord.y < 0 || neighborCoord.y >= 32 || neighborCoord.z < 0 || neighborCoord.z >= 16)
            return 0;
        else
        {
            return getCube(neighborCoord.x, neighborCoord.y, neighborCoord.z, cube);
        }
    }
};

#endif