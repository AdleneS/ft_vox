#ifndef CHUNK_H
#define CHUNK_H

#include "../gl3w/include/GL/gl3w.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "cube.hpp"
class Chunk
{
public:
	int Id;
	glm::vec3 Position;
	glm::mat4x4 mat;
	Cube ***CubeData;
	std::vector<float> Vertices;
	std::vector<float> UV;
	std::vector<glm::vec2> texCoord;
	std::vector<float> Normal;
	int size;
	GLuint VAO;
	GLuint VBO;
	GLuint UVB;
	GLuint TOB;
	GLuint NBO;

	Chunk(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), int id = -1);
	~Chunk();
	void initCubeData();
	void memCubeData();
	void freeCubeData();
	void translate(glm::vec3 v);
	void loadVBO();

private:
};

#endif