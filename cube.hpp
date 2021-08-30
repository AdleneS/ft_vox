#ifndef CUBE_H
#define CUBE_H

#include "gl3w/include/GL/gl3w.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <vector>

// Default camera values
static float VERTICES[] = {
	//NORTH
	-1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,
	//EAST
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	//SOUTH
	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	//WEST
	-1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	//TOP
	-1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, -1.0f,
	//BOT
	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, -1.0f};

static float UV[] =
	{
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,

		0.0f, 0.0f,
		-1.0f, 0.0f,
		-1.0f, -1.0f,
		-1.0f, -1.0f,
		0.0f, -1.0f,
		0.0f, 0.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,

		0.0f, 0.0f,
		-1.0f, -0.0f,
		-1.0f, -1.0f,
		-1.0f, -1.0f,
		0.0f, -1.0f,
		0.0f, 0.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f};

static int SIZEV = 432;
static int SIZEUV = 288;
class Cube
{
public:
	int Id;
	glm::vec3 Position;
	glm::mat4 mat;
	bool isEmpty;
	glm::vec2 texCoord;

	Cube(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), bool isempty = false, int id = -1, glm::vec2 texcoord = glm::vec2(0, 0))
	{
		Position = position;
		Id = id;
		mat = glm::mat4(1.0f);
		isEmpty = isempty;
		texCoord = texcoord;
		translate(Position);
	}
	//Cube(float posX, float posY, float posZ)
	//{
	//	Position = glm::vec3(posX, posY, posZ);
	//}
	void translate(glm::vec3 v)
	{
		mat = glm::translate(mat, v);
	}

private:
};
#endif