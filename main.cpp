
#include <filesystem>
#include "vox.hpp"
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.hpp"

Camera camera(glm::vec3(0.0f, 256.0f, 0.0f));

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
	vox->chunkNbX = 4;
	vox->chunkNbZ = 2;
	vox->chunkCount = 0;
	vox->seed = rand();
	return vox;
}

void createExpendedChunkX(t_vox *vox, std::vector<Chunk *> *chunks, float x, float z, int o)
{

	chunks->push_back(new Chunk(createCube(vox, o, glm::vec3(x * CHUNK_SIZE_X, 0, z * CHUNK_SIZE_Z), vox->seed)));
	chunks->back()->loadVBO();
	chunks->back()->Vertices.clear();
	chunks->back()->Vertices.shrink_to_fit();
	chunks->back()->freeCubeData();
	vox->chunkCount++;
	vox->chunkNbX++;
}

int main(void)
{

	GLFWwindow *window;
	std::vector<Chunk *> chunks;
	t_vox *vox = init();

	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ft_vox", NULL, NULL);

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

	Shader shader("C:/Users/wks/Desktop/ft_vox/vertex.glsl", "C:/Users/wks/Desktop/ft_vox/fragment.glsl");
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window, key_callback);

	shader.use();
	load_texture("C:/Users/wks/Desktop/ft_vox/resources/textures/textures2.jpg");
	shader.setInt("texture", 0);
	glEnable(GL_DEPTH_TEST);
	//createChunk(vox);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.69f, 0.94f, 0.95f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		displayChunk(shader, vox, &chunks);
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
		shader.setMat4("projection", projection);
		glm::mat4 view = camera.GetViewMatrix();
		shader.setMat4("view", view);

		//printf("%f | %f | %f \n", camera.Position.x, camera.Position.y, camera.Position.z);

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
	glm::vec2 tex = glm::vec2(0, 0);
	for (size_t x = 0; x < CHUNK_SIZE_X; x++)
	{
		for (size_t y = 0; y < CHUNK_SIZE_Y; y++)
		{
			for (size_t z = 0; z < CHUNK_SIZE_Z; z++)
			{
				if (chunk->CubeData[x][y][z].isEmpty || chunk->CubeData[x][y][z].Id < 0)
					continue;
				for (int i = 0; i < 6; i++)
				{
					if (i == 4)
						tex = glm::vec2(1, 0);
					else if (i == 5)
						tex = glm::vec2(2, 0);
					else
						tex = glm::vec2(0, 0);
					if (y < CHUNK_SIZE_Y - 10)
						tex = glm::vec2(0, 1);
					else if (y < CHUNK_SIZE_Y - 7)
						tex = glm::vec2(1, 1);

					if (!mesh.getNeighbor(x, y, z, (Direction)i, chunk->CubeData))
					{
						for (size_t j = 0; j < 18; j += 3)
						{
							chunk->Vertices.push_back(VERTICES[j + (i * 18)] + chunk->CubeData[x][y][z].Position.x);
							chunk->Vertices.push_back(VERTICES[j + 1 + (i * 18)] + chunk->CubeData[x][y][z].Position.y);
							chunk->Vertices.push_back(VERTICES[j + 2 + (i * 18)] + chunk->CubeData[x][y][z].Position.z);
							chunk->texCoord.push_back(tex);
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
			}
		}
	}
}

Chunk createCube(t_vox *vox, int chunkId, glm::vec3 offsets, int seed)
{
	int id = 0;

	Chunk chunk(offsets, chunkId);

	for (size_t x = 0; x < CHUNK_SIZE_X; x++)
	{
		for (size_t z = 0; z < CHUNK_SIZE_Z; z++)
		{
			float n = 0.5;
			float a = 0.55;
			float freq = 0.0016;
			int isEmpty = false;
			double value = 0;
			for (int octave = 0; octave < 8; octave++)
			{
				value = a * glm::simplex(glm::vec2((x + offsets.x + seed) * freq, (z + offsets.z + seed) * freq));
				n += value;
				a *= 0.5;
				freq *= 2.0;
			}

			n = (n + 1) / 2;

			n *= CHUNK_SIZE_Y - 1;
			if (n >= CHUNK_SIZE_Y - 1)
				n = CHUNK_SIZE_Y - 1;
			if (n <= CHUNK_SIZE_Y - 10)
				n = CHUNK_SIZE_Y - 11;

			//n = round(n);
			//if ((int)n % 2 != 0)
			//	n += 1;
			//if (value >= CHUNK_SIZE_X || value >= CHUNK_SIZE_Y || value >= CHUNK_SIZE_Z)
			//	isEmpty = true;
			//printf("%f || ", value);
			//printf("%f || ", value);
			chunk.CubeData[x][(int)n][z].Position = glm::vec3((x), ((int)n), (z));
			chunk.CubeData[x][(int)n][z].Id = id;
			chunk.CubeData[x][(int)n][z].isEmpty = false;
			chunk.CubeData[x][(int)n][z].value = n;

			id = id + 1 + chunkId * (vox->chunkNbX + vox->chunkNbZ);
		}
	}

	createMesh(&chunk);
	return chunk;
}

void displayChunk(Shader shader, t_vox *vox, std::vector<Chunk *> *chunks)
{
	int new_view_distance_x = (VIEW_DISTANCE / 2) + ceil((int)camera.Position.x / CHUNK_SIZE_X);
	int new_view_distance_z = (VIEW_DISTANCE / 2) + ceil((int)camera.Position.z / CHUNK_SIZE_Z);
	size_t o = 0;

	if (chunks->size() > 0)
		for (auto i = 0; i < chunks->size(); i++)
		{
			//printf("%d\n", chunks->size());
			shader.setMat4("model", chunks->at(i)->mat);
			glBindVertexArray(chunks->at(i)->VAO);
			glDrawArrays(GL_TRIANGLES, 0, chunks->at(i)->size);
		}

	//printf("%f\n", -VIEW_DISTANCE + ceil((int)camera.Position.x / 16));
	for (int x = (-VIEW_DISTANCE / 2) + ceil((int)camera.Position.x / CHUNK_SIZE_X); x < new_view_distance_x; x++)
	{
		for (int z = (-VIEW_DISTANCE / 2) + ceil((int)camera.Position.z / CHUNK_SIZE_Z); z < new_view_distance_z; z++)
		{
			int find = 0;
			if (chunks->size() == 0)
			{
				createExpendedChunkX(vox, chunks, (float)x, (float)z, o);
				continue;
			}
			for (auto i = 0; i < chunks->size(); i++)
			{
				shader.setMat4("model", chunks->at(i)->mat);
				glBindVertexArray(chunks->at(i)->VAO);
				glDrawArrays(GL_TRIANGLES, 0, chunks->at(i)->size);
				//printf("%f %f\n", round(chunks->at(i)->Position.x), round(x * CHUNK_SIZE_X));
				if (round(chunks->at(i)->Position.x) == round(x * CHUNK_SIZE_X) && round(chunks->at(i)->Position.z) == round(z * CHUNK_SIZE_Z))
				{
					find = 1;
					break;
				}
			}
			if (!find)
				createExpendedChunkX(vox, chunks, x, z, o);
			o++;
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