
#include <filesystem>
#include "vox.hpp"
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.hpp"

void createCube(Cube ***cube, Shader shader, glm::vec3 offset);

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f; // time between current frame and last frame
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

	Chunk *chunk;

	chunk = (Chunk *)malloc(sizeof(Chunk) * CHUNK_NB * CHUNK_NB);

	int o = 0;
	for (size_t x = 0; x < CHUNK_NB; x++)
	{
		for (size_t z = 0; z < CHUNK_NB; z++)
		{
			chunk[o] = createChunk(o, glm::vec3(x * CHUNK_SIZE_X * 2, 0, z * CHUNK_SIZE_Z * 2), rand());
			o++;
		}
	}

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

	//std::vector<glm::vec3> ccc;
	//ccc = (faceVertices(ccc, 0));
	//ccc = (faceVertices(ccc, 1));
	//printf("%f %f %f \n", ccc[4].x, ccc[4].y, ccc[4].z);
	//ccc.push_back(faceVertices((int)Direction::South));
	//ccc.push_back(faceVertices((int)Direction::West));
	//ccc.push_back(faceVertices((int)Direction::Top));
	//ccc.push_back(faceVertices((int)Direction::Bot));

	unsigned int VBO, VAO, UVB, IBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);
	glGenBuffers(1, &UVB);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, SIZEV, VERTICES, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, UVB);
	glBufferData(GL_ARRAY_BUFFER, SIZEUV, UV, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(1);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
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
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		glBindVertexArray(VAO);
		displayChunk(chunk, shader);
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
		shader.setMat4("projection", projection);
		glm::mat4 view = camera.GetViewMatrix();
		shader.setMat4("view", view);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &UVB);
	free(chunk);
	glfwDestroyWindow(window);

	glfwTerminate();
	exit(EXIT_SUCCESS);
}

void createCube(Cube ***cube, Shader shader, glm::vec3 offset)
{
	Mesh mesh;
	for (size_t x = 0; x < CHUNK_SIZE_X; x++)
	{
		for (size_t y = 0; y < CHUNK_SIZE_Y; y++)
		{
			for (size_t z = 0; z < CHUNK_SIZE_Z; z++)
			{
				if (!mesh.getCube(x, y, z, cube))
					continue;
				shader.setMat4("model", glm::translate(cube[x][y][z].mat, offset));
				displayCube(x, y, z, shader, cube);
			}
		}
	}
}

void displayCube(int x, int y, int z, Shader shader, Cube ***cube)
{
	Mesh mesh;
	//std::vector<float> a;
	for (int i = 0; i < 6; i++)
	{
		if (!mesh.getNeighbor(x, y, z, (Direction)i, cube))
		{
			//for (size_t j = 0; j < 18; j++)
			//{
			//	mesh.a.push_back(VERTICES[j + (i * 18)]);
			//}

			shader.setVec2("spriteID", cube[x][y][z].texCoord);
			if (i == 4)
				shader.setVec2("spriteID", glm::vec2(1, 0));
			if (i == 5)
				shader.setVec2("spriteID", glm::vec2(2, 0));
			glBindTexture(GL_TEXTURE_2D, 1);
			glDrawArrays(GL_TRIANGLES, 6 * i, 6);
		}
	}
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.a.size() * sizeof(GLfloat), &mesh.a[0], GL_DYNAMIC_DRAW);
	//glDrawElements(GL_TRIANGLES, mesh.a.size(), GL_UNSIGNED_INT, nullptr);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.nbIndices * sizeof(GLfloat), &mesh.a[0], GL_DYNAMIC_DRAW);
	//glDrawElements(GL_TRIANGLES, mesh.nbIndices, GL_UNSIGNED_INT, nullptr);
}

Chunk createChunk(int chunkId, glm::vec3 offsets, int seed)
{
	int id = 0;

	// Cube* cube = (int*)malloc(M * N * O * sizeof(int));
	Cube ***cube = (Cube ***)malloc(CHUNK_SIZE_X * sizeof(Cube **));

	for (int i = 0; i < CHUNK_SIZE_X; i++)
	{
		cube[i] = (Cube **)malloc(CHUNK_SIZE_Y * sizeof(Cube *));

		for (int j = 0; j < CHUNK_SIZE_Y; j++)
		{
			cube[i][j] = (Cube *)malloc(CHUNK_SIZE_Z * sizeof(Cube));
		}
	}

	for (size_t x = 0; x < CHUNK_SIZE_X; x++)
	{
		for (size_t y = 0; y < CHUNK_SIZE_Y; y++)
		{
			for (size_t z = 0; z < CHUNK_SIZE_Z; z++)
			{
				int isEmpty = false;
				float value = glm::simplex(glm::vec2((x + offsets.x * seed) / 64, (z + offsets.z * seed) / 64));
				value = (value + 1) / 2;

				value *= CHUNK_SIZE_Y;
				value = round(value);
				if (y >= value)
					isEmpty = true;

				Cube c(glm::vec3(2 * x, y * 2, 2 * z), isEmpty, id);
				cube[x][y][z] = c;
				id++;
			}
		}
	}
	Chunk chunk(cube, chunkId - 1);
	return chunk;
}

void displayChunk(Chunk *chunk, Shader shader)
{
	int o = 0;
	for (size_t x = 0; x < CHUNK_NB; x++)
	{
		for (size_t z = 0; z < CHUNK_NB; z++)
		{
			createCube(chunk[o].CubeData, shader, glm::vec3(x * CHUNK_SIZE_X * 2, 0 * CHUNK_SIZE_Y * 2, z * CHUNK_SIZE_Z * 2));
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