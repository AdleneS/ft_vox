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
	glm::mat4x4 mat;
	Cube ***CubeData;
	std::vector<float> Vertices;
	std::vector<float> UV;
	std::vector<glm::vec2> texCoord;
	int size;
	int sizeUV;

	GLuint VAO;
	GLuint VBO;
	GLuint UVB;
	GLuint TOB;

	Chunk(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), int id = -1)
	{
		Id = id;
		size = 0;
		sizeUV = 0;
		mat = glm::mat4(1.0f);
		Position = position;
		translate(Position);
		initCubeData();
	}
	~Chunk();
	void initCubeData()
	{
		CubeData = (Cube ***)malloc(CHUNK_SIZE_X * sizeof(Cube **));

		for (int i = 0; i < CHUNK_SIZE_X; i++)
		{
			CubeData[i] = (Cube **)malloc(CHUNK_SIZE_Y * sizeof(Cube *));

			for (int j = 0; j < CHUNK_SIZE_Y; j++)
			{
				CubeData[i][j] = (Cube *)malloc(CHUNK_SIZE_Z * sizeof(Cube));
			}
		}
	}
	void freeCubeData()
	{
		for (int i = 0; i < CHUNK_SIZE_X; i++)
		{
			for (int j = 0; j < CHUNK_SIZE_Y; j++)
			{
				free(CubeData[i][j]);
			}
			free(CubeData[i]);
		}
		free(CubeData);
	}
	void translate(glm::vec3 v)
	{
		mat = glm::translate(mat, v);
	}

	void loadVBO()
	{
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &UVB);
		glGenBuffers(1, &TOB);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, size * sizeof(float), &Vertices[0], GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, UVB);
		glBufferData(GL_ARRAY_BUFFER, sizeUV * sizeof(float), &UV[0], GL_DYNAMIC_DRAW);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, TOB);
		glBufferData(GL_ARRAY_BUFFER, size * sizeof(float), &texCoord[0], GL_DYNAMIC_DRAW);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
		glEnableVertexAttribArray(2);
	}

private:
};

Chunk::~Chunk()
{
	Vertices.clear();
	Vertices.shrink_to_fit();
}
#endif