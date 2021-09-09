#ifndef MESH_H
#define MESH_H

#include "vox.hpp"
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
    Mesh()
    {
    }
    int getCube(Cube neighbor, Cube current, int dir)
    {
        if (neighbor.Id < 0 || neighbor.isEmpty)
            return 0;
        switch (int(dir))
        {
        case 0:
            if (neighbor.Position.z - 1 == current.Position.z)
                return 1;
            break;
        case 1:
            if (neighbor.Position.x - 1 == current.Position.x)
                return 1;
            break;
        case 2:
            if (neighbor.Position.z == current.Position.z - 1)
                return 1;
            break;
        case 3:
            if (neighbor.Position.x == current.Position.x - 1)
                return 1;
        case 4:
            if (neighbor.Position.y - 1 == current.Position.y)
                return 1;
            break;
        case 5:
            if (neighbor.Position.y == current.Position.y - 1)
                return 1;
            break;
        default:
            return 0;
            break;
        }
        return 0;
    }
    int getNeighbor(int x, int y, int z, Direction dir, Cube ***cube)
    {
        CubeCoordinate offsetToCheck = offsets[(int)dir];
        CubeCoordinate nCoord = {x + offsetToCheck.x, y + offsetToCheck.y, z + offsetToCheck.z};
        if (nCoord.x < 0 || nCoord.x >= CHUNK_SIZE_X || nCoord.y < 0 || nCoord.y >= CHUNK_SIZE_Y || nCoord.z < 0 || nCoord.z >= CHUNK_SIZE_Z)
            return 0;
        else
        {
            /*if (cube[nCoord.x][nCoord.y][nCoord.z].value < cube[x][y][z].value)
            {
                nCoord.x += cube[x][y][z].value / 2;
                nCoord.y += cube[x][y][z].value / 2;
                nCoord.z += cube[x][y][z].value / 2;
            }
            else if (cube[nCoord.x][nCoord.y][nCoord.z].value > cube[x][y][z].value)
            {
                float value = cube[nCoord.x][nCoord.y][nCoord.z].value;
                nCoord.x -= value / 2;
                nCoord.y -= value / 2;
                nCoord.z -= value / 2;
            }
            if (nCoord.x < 0 || nCoord.x >= CHUNK_SIZE_X || nCoord.y < 0 || nCoord.y >= CHUNK_SIZE_Y || nCoord.z < 0 || nCoord.z >= CHUNK_SIZE_Z)
                return 0;*/
            return getCube(cube[nCoord.x][nCoord.y][nCoord.z], cube[x][y][z], (int)dir);
        }
    }
};

#endif