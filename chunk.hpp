#ifndef CHUNK_H
#define CHUNK_H

#include "gl3w/include/GL/gl3w.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "cube.hpp"

class Chunk
{
public:
	int Id;
	glm::vec3 Position;
	Cube ***CubeData;
	Chunk(Cube ***cube, int id = -1, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f))
	{
		Id = id;
		CubeData = cube;
	}

private:
};
#endif