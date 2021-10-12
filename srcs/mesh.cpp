#include "../headers/mesh.hpp"

Mesh::Mesh(glm::vec3 position)
{
    mat = glm::mat4(1.0f);
    Position = position;
    size = 0;
    translate(Position);
}
Mesh::~Mesh()
{
    Vertices.clear();
    Vertices.shrink_to_fit();
    UV.clear();
    UV.shrink_to_fit();
    texCoord.clear();
    texCoord.shrink_to_fit();
    Normal.clear();
    Normal.shrink_to_fit();
}
void Mesh::translate(glm::vec3 v)
{
    mat = glm::translate(mat, v);
}

int Mesh::getCube(Cube neighbor, Cube current, int dir)
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
int Mesh::getNeighbor(int x, int y, int z, Direction dir, Cube ***cube)
{
    CubeCoordinate offsetToCheck = offsets[(int)dir];
    CubeCoordinate nCoord = {x + offsetToCheck.x, y + offsetToCheck.y, z + offsetToCheck.z};
    if (nCoord.x < 0 || nCoord.x >= CHUNK_SIZE_X || nCoord.y < 0 || nCoord.y >= CHUNK_SIZE_Y || nCoord.z < 0 || nCoord.z >= CHUNK_SIZE_Z)
        return 0;
    else
        return getCube(cube[nCoord.x][nCoord.y][nCoord.z], cube[x][y][z], (int)dir);
}