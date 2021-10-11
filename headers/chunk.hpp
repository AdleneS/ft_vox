#ifndef CHUNK_H
#define CHUNK_H

#include "../gl3w/include/GL/gl3w.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "cube.hpp"
#include "const.hpp"
#include <cstdio>
class Chunk
{
public:
	int Id;
	bool rendered;
	glm::vec3 Position;
	int maxHeight;
	Cube ***CubeData;

	Chunk(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), int id = -1);
	~Chunk();
	void initCubeData();
	void memCubeData();
	void freeCubeData();
	void translate(glm::vec3 v);

private:
};

#endif