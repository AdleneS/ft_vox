#ifndef CUBE_H
#define CUBE_H

#include "gl3w/include/GL/gl3w.h"
#include "glm/glm.hpp"

#include <vector>


// Default camera values
float VERTICES[] = {
			//NORTH
			-1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
			 1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, -1.0f, 0.0f, 1.0f,
			 1.0f,  1.0f, -1.0f, 0.0f, 1.0f,
			-1.0f,  1.0f, -1.0f, 1.0f, 1.0f,
			-1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
			//SOUTH
			-1.0f, -1.0f,  1.0f, 0.0f, 0.0f,
			 1.0f, -1.0f,  1.0f, 1.0f, 0.0f,
			 1.0f,  1.0f,  1.0f, 1.0f, 1.0f,
			 1.0f,  1.0f,  1.0f, 1.0f, 1.0f,
			-1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
			-1.0f, -1.0f,  1.0f, 0.0f, 0.0f,
			//WEST
			-1.0f,  1.0f,  1.0f, 0.0f, 0.0f,
			-1.0f,  1.0f, -1.0f, -1.0f, 0.0f,
			-1.0f, -1.0f, -1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f, 0.0f, -1.0f,
			-1.0f,  1.0f,  1.0f, 0.0f, 0.0f,
			//EAST
			 1.0f,  1.0f,  1.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, -1.0f, -1.0f, -0.0f,
			 1.0f, -1.0f, -1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f, 0.0f, -1.0f,
			 1.0f,  1.0f,  1.0f, 0.0f, 0.0f,
			//BOT
			-1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
			 1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
			 1.0f, -1.0f,  1.0f, 1.0f, 1.0f,
			 1.0f, -1.0f,  1.0f, 1.0f, 1.0f,
			-1.0f, -1.0f,  1.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
			//TOP
			-1.0f,  1.0f, -1.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, -1.0f, 1.0f, 0.0f,
			 1.0f,  1.0f,  1.0f, 1.0f, 1.0f,
			 1.0f,  1.0f,  1.0f, 1.0f, 1.0f,
			-1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
			-1.0f,  1.0f, -1.0f, 0.0f, 0.0f
};

class Cube
{
public:
   
	glm::vec3	Position;
	float       *Vertices;
	int         size;
	glm::mat4	model;

	Cube(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), float *vertices = VERTICES)
	{
		Position = position;
		Vertices = vertices;
		size = sizeof(VERTICES);
	}
	Cube(float posX, float posY, float posZ)
	{
		Position = glm::vec3(posX, posY, posZ);
	}
	void translate(glm::vec3 v)
	{
		model = glm::translate(model, v);
		//std::cout << v.x << " " << v.y << " " << v.z <<" \n"; 
	}
private:
};
#endif