
#include <filesystem>
#include "vox.hpp"
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.hpp"

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

static void error_callback(int error, const char *description)
{
	(void)error;
	fputs(description, stderr);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	(void)scancode;
	(void)mods;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

t_vox *init()
{
	t_vox *vox;
	vox = (t_vox *)malloc(sizeof(t_vox));
	vox->chunk = (Chunk *)malloc(sizeof(Chunk) * CHUNK_NB * CHUNK_NB * CHUNK_NB);

	return vox;
}

void createChunk(t_vox *vox)
{
	int o = 0;
	for (size_t x = 0; x < CHUNK_NB; x++)
	{
		for (size_t y = 0; y < CHUNK_NB_Y; y++)
		{
			for (size_t z = 0; z < CHUNK_NB; z++)
			{
				vox->chunk[o] = createCube(o, glm::vec3(x * CHUNK_SIZE_X * 2, y * CHUNK_SIZE_Y * 2, z * CHUNK_SIZE_Z * 2), rand());
				vox->chunk[o].loadVBO();
				vox->chunk[o].Vertices.clear();
				vox->chunk[o].Vertices.shrink_to_fit();
				vox->chunk[o].freeCubeData();
				o++;
			}
		}
	}
}

int main(void)
{

	GLFWwindow *window;
	Cube cube;
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ft_vox", NULL, NULL);

	t_vox *vox = init();

	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	gl3wInit();
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glEnable(GL_DEPTH_TEST);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	//glBindVertexArray(0);

	Shader shader("C:/Users/wks/Desktop/ft_vox/vertex.glsl", "C:/Users/wks/Desktop/ft_vox/fragment.glsl");
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window, key_callback);

	shader.use();
	load_texture("C:/Users/wks/Desktop/ft_vox/resources/textures/textures.jpg");
	shader.setInt("texture", 0);
	glEnable(GL_DEPTH_TEST);
	createChunk(vox);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		displayChunk(vox->chunk, shader, vox);
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
		shader.setMat4("projection", projection);
		glm::mat4 view = camera.GetViewMatrix();
		shader.setMat4("view", view);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	//glDeleteBuffers(1, &vox->VBO);
	//glDeleteBuffers(1, &vox->UVB);
	//glDeleteBuffers(1, &vox->TOB);
	//free(vox->chunk);
	glfwDestroyWindow(window);

	glfwTerminate();
	exit(EXIT_SUCCESS);
}

void createMesh(Chunk *chunk)
{
	Mesh mesh;
	for (size_t x = 0; x < CHUNK_SIZE_X; x++)
	{
		for (size_t y = 0; y < CHUNK_SIZE_Y; y++)
		{
			for (size_t z = 0; z < CHUNK_SIZE_Z; z++)
			{
				//if (!chunk->CubeData[x][y][z].isEmpty && chunk->CubeData[x][y][z].Id >= 0)
				for (int i = 0; i < 6; i++)
				{
					if (!mesh.getNeighbor(x, y, z, (Direction)i, chunk->CubeData))
					{
						for (size_t j = 0; j < 18; j += 3)
						{
							chunk->texCoord.push_back(glm::vec2(0, 0));
							chunk->texCoord.push_back(glm::vec2(0, 0));
							chunk->texCoord.push_back(glm::vec2(0, 0));
							chunk->Vertices.push_back(VERTICES[j + (i * 18)] + chunk->CubeData[x][y][z].Position.x);
							chunk->Vertices.push_back(VERTICES[j + 1 + (i * 18)] + chunk->CubeData[x][y][z].Position.y);
							chunk->Vertices.push_back(VERTICES[j + 2 + (i * 18)] + chunk->CubeData[x][y][z].Position.z);
							chunk->size += 3;
						}
						for (size_t k = 0; k < 12; k += 2)
						{
							chunk->UV.push_back(UV[k + (i * 12)]);
							chunk->UV.push_back(UV[k + 1 + (i * 12)]);
							chunk->sizeUV += 2;
						}
					}
				}

				//if (!mesh.getCube(x, y, z, cube))
				//	continue;
			}
		}
	}
}

Chunk createCube(int chunkId, glm::vec3 offsets, int seed)
{
	int id = 0;

	Chunk chunk(offsets, chunkId);

	for (size_t x = 0; x < CHUNK_SIZE_X; x++)
	{
		for (size_t y = 0; y < CHUNK_SIZE_Y; y++)
		{
			for (size_t z = 0; z < CHUNK_SIZE_Z; z++)
			{
				int isEmpty = false;
				float value = glm::simplex(glm::vec2((x + offsets.x + seed) / 16, (z + offsets.z + seed) / 16));
				value = (value + 1) / 2;

				value *= 8;
				value = round(value);
				if ((int)value % 2 != 0)
					value += 1;
				//printf("%f \n", value);
				//if (value >= CHUNK_SIZE_Y)
				//	isEmpty = true;
				//printf("%f || ", value);
				value = 0;
				chunk.CubeData[x][y][z].Position = glm::vec3(2 * x, (y * 2) + value, 2 * z);
				chunk.CubeData[x][y][z].Id = id;
				chunk.CubeData[x][y][z].isEmpty = isEmpty;

				id = id + 1 + chunkId * CHUNK_NB;
			}
		}
	}

	createMesh(&chunk);
	//chunk.loadVBO();

	//for (int i = 0; i < CHUNK_SIZE_X; i++)
	//{
	//	for (int j = 0; j < CHUNK_SIZE_Y; j++)
	//	{
	//		free(cube[i][j]);
	//	}
	//	free(cube[i]);
	//}
	//free(cube);
	return chunk;
}

void displayChunk(Chunk *chunk, Shader shader, t_vox *vox)
{
	int o = 0;
	for (size_t x = 0; x < CHUNK_NB; x++)
	{
		for (size_t y = 0; y < CHUNK_NB_Y; y++)
		{
			for (size_t z = 0; z < CHUNK_NB; z++)
			{
				shader.setMat4("model", chunk[o].mat);
				glBindVertexArray(chunk[o].VAO);
				//glBindBuffer(GL_ARRAY_BUFFER, chunk[o].VBO);
				glDrawArrays(GL_TRIANGLES, 0, chunk[o].size);
				//glDrawArrays(GL_TRIANGLES, 0, 36);
				o++;
			}
		}
	}
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		camera.MovementSpeed = 20;
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE)
		camera.MovementSpeed = 10;
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
	(void)window;
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
	(void)window;
	(void)xoffset;
	camera.ProcessMouseScroll(yoffset);
}

GLuint load_texture(const char *imagePath)
{
	unsigned int texture1;

	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char *data = stbi_load(imagePath, &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	return texture1;
}

//std::array<int,