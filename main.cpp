#include <filesystem>
#include "vox.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.hpp"

typedef std::pair<vec3, Chunk *> PosChunk;
Camera camera(glm::vec3(0.0f, 128.0f, 0.0f));

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
	srand((unsigned int)time(NULL));

	t_vox *vox;
	vox = (t_vox *)malloc(sizeof(t_vox));
	vox->chunkNbX = 4;
	vox->chunkNbZ = 2;
	vox->chunkCount = 0;
	vox->seed = rand() % 10000;
	return vox;
}

void fps(GLFWwindow *window)
{
	static double current_time = 0;
	static double last_time = 0;
	char buffer[5];
	char title[] = "VOX | ";
	last_time = current_time;
	current_time = glfwGetTime();
	double fps = 1.0 / (current_time - last_time);
	snprintf(buffer, sizeof buffer, "%f", fps);
	glfwSetWindowTitle(window, std::strcat(title, buffer));
}

int createExpendedChunkX(t_vox *vox, std::unordered_map<vec3, Chunk *, MyHashFunction> *chunks, int x, int z, int o)
{
	if (chunks->find(vec3(x, 0, z)) == chunks->end())
	{
		chunks->emplace(PosChunk(vec3(x, 0, z), new Chunk(createCube(vox, o, vec3(x * CHUNK_SIZE_X, 0, z * CHUNK_SIZE_Z), vox->seed))));
		chunks->at(vec3(x, 0, z))->loadVBO();
		chunks->at(vec3(x, 0, z))->Vertices.clear();
		chunks->at(vec3(x, 0, z))->Vertices.shrink_to_fit();
		vox->chunkCount++;
		vox->chunkNbX++;
		return 1;
	}
	return 0;
}

int main(void)
{

	GLFWwindow *window;
	std::unordered_map<vec3, Chunk *, MyHashFunction> chunks;
	t_vox *vox = init();

	printf("%d\n", vox->seed);
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
	load_texture("C:/Users/wks/Desktop/ft_vox/resources/textures/textures.jpg");
	shader.setInt("texture", 0);
	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window))
	{
		Frustum frustum;
		fps(window);
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.69f, 0.94f, 0.95f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
		shader.setMat4("projection", projection);
		glm::mat4 view = camera.GetViewMatrix();
		shader.setMat4("view", view);
		frustum.Transform(projection, view);
		shader.setVec3("lightPos", camera.Position);
		shader.setVec3("viewPos", camera.Position);
		displayChunk(shader, vox, &chunks, frustum);
		glfwSwapBuffers(window);
		glfwPollEvents();
		createChunk(vox, &chunks);
	}
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
				if (chunk->CubeData[x][y][z].isEmpty)
					continue;
				for (int i = 0; i < 6; i++)
				{
					tex = chunk->CubeData[x][y][z].texCoord;
					if (tex.x == 0 && tex.y == 0 && i == 4)
						tex = glm::vec2(1, 0);
					if (tex.x == 0 && tex.y == 0 && i == 5)
						tex = glm::vec2(2, 0);
					if (!mesh.getNeighbor(x, y, z, (Direction)i, chunk->CubeData))
					{
						for (size_t j = 0; j < 18; j += 3)
						{
							chunk->Vertices.push_back(VERTICES[j + (i * 18)] + chunk->CubeData[x][y][z].Position.x);
							chunk->Vertices.push_back(VERTICES[j + 1 + (i * 18)] + chunk->CubeData[x][y][z].Position.y);
							chunk->Vertices.push_back(VERTICES[j + 2 + (i * 18)] + chunk->CubeData[x][y][z].Position.z);

							chunk->Normal.push_back(NORMAL[j + (i * 18)]);
							chunk->Normal.push_back(NORMAL[j + 1 + (i * 18)]);
							chunk->Normal.push_back(NORMAL[j + 2 + (i * 18)]);
							chunk->texCoord.push_back(tex);
							chunk->size += 3;
						}
						for (size_t k = 0; k < 12; k += 2)
						{
							chunk->UV.push_back(UV[k + (i * 12)]);
							chunk->UV.push_back(UV[k + 1 + (i * 12)]);
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
	glm::vec2 texCoord = glm::vec2(0, 0);

	Chunk chunk(offsets, chunkId);
	for (size_t x = 0; x < CHUNK_SIZE_X; x++)
	{
		for (size_t z = 0; z < CHUNK_SIZE_Z; z++)
		{
			float n = 0.5;
			float a = 0.3;
			float freq = 0.0032;
			float value = 0;
			for (int octave = 0; octave < 8; octave++)
			{
				value = a * glm::simplex(glm::vec2((x + offsets.x + seed) * freq, (z + offsets.z + seed) * freq)); // Get2DPerlinNoiseValue((x + offsets.x + (float)seed) * freq, (z + offsets.z + (float)seed) * freq, 1);
				n += value;
				a *= 0.5;
				freq *= 2.0;
			}

			n = (n + 1) / 2;

			n *= CHUNK_SIZE_Y / 2;
			if (n >= CHUNK_SIZE_Y - 1)
				n = CHUNK_SIZE_Y - 1;
			if (n <= CHUNK_SIZE_Y / 3)
				n = CHUNK_SIZE_Y / 3 - 1;

			chunk.CubeData[x][(int)n][z].Position = glm::vec3((x), (int)(n), (z));
			chunk.CubeData[x][(int)n][z].Id = id;
			chunk.CubeData[x][(int)n][z].texCoord = selectTex(n);
			chunk.CubeData[x][(int)n][z].isEmpty = false;
			chunk.CubeData[x][(int)n][z].value = n;

			id = id + 1 + chunkId * (vox->chunkNbX + vox->chunkNbZ);

			for (int y = 50; y < (int)n; y++)
			{
				float nn = 0.5;
				a = 0.7;
				freq = 0.0632;
				value = 0;
				for (int octave = 0; octave < 2; octave++)
				{
					value = a * glm::simplex(glm::vec3((x + offsets.x + (float)seed) * freq, (y + (float)seed) * freq, (z + offsets.z + (float)seed) * freq)); //glm::simplex(glm::vec2((x + offsets.x + seed) * freq, (z + offsets.z + seed) * freq))
					nn += value;
					a *= 0.5;
					freq *= 2.0;
				}

				nn = (nn + 1) / 2;
				//if (value < n - 10)
				if (nn > 0.8)
				{
					chunk.CubeData[x][(y)][z].Position = glm::vec3((x), (y), (z));
					chunk.CubeData[x][(y)][z].Id = id;
					chunk.CubeData[x][(y)][z].texCoord = glm::vec2(0, 2);
					chunk.CubeData[x][(y)][z].isEmpty = false;
					chunk.CubeData[x][(y)][z].value = y;
				}
				if (y == 50)
				{
					chunk.CubeData[x][(y)][z].Position = glm::vec3((x), (y), (z));
					chunk.CubeData[x][(y)][z].Id = id;
					chunk.CubeData[x][(y)][z].texCoord = glm::vec2(0, 2);
					chunk.CubeData[x][(y)][z].isEmpty = false;
					chunk.CubeData[x][(y)][z].value = y;
				}
			}
		}
	}
	createMesh(&chunk);
	chunk.freeCubeData();
	return chunk;
}
glm::vec2 selectTex(int n)
{
	if (n < CHUNK_SIZE_Y / 3)
		return glm::vec2(0, 1);
	if (n < CHUNK_SIZE_Y / 3 + 3)
		return glm::vec2(1, 1);
	return glm::vec2(0, 0);
}
void displayChunk(Shader shader, t_vox *vox, std::unordered_map<vec3, Chunk *, MyHashFunction> *chunks, Frustum frustum)
{
	std::vector<vec3> vec;
	int triNb = 0;
	if (chunks->size() > 0)
	{
		glfwPollEvents();

		for (auto it = chunks->begin(); it != chunks->end(); ++it)
		{
			int distanceFromChunk = sqrt(pow((camera.Position.x - (it->second->Position.x + 8)), 2) + pow((camera.Position.z - (it->second->Position.z + 8)), 2));

			if (distanceFromChunk > VIEW_DISTANCE * CHUNK_SIZE_X * 1.4)
			{
				vec.emplace_back(it->first);
				continue;
			}
			else if (distanceFromChunk > VIEW_DISTANCE * VIEW_DISTANCE || frustum.IsInside(*it->second) == Frustum::Invisible)
			{
				continue;
			}
			else
			{
				shader.setMat4("model", it->second->mat);
				glBindVertexArray(it->second->VAO);
				glDrawArrays(GL_TRIANGLES, 0, it->second->size / 3);
				triNb += it->second->size / 3;
			}
		}
		for (auto &&key : vec)
		{
			delete (chunks->find(key)->second);
			chunks->erase(key);
		}
		printf("%d\n", triNb);
	}
}

void createChunk(t_vox *vox, std::unordered_map<vec3, Chunk *, MyHashFunction> *chunks)
{
	int new_view_distance_x = (VIEW_DISTANCE) + (int)((int)camera.Position.x / CHUNK_SIZE_X);
	int new_view_distance_z = (VIEW_DISTANCE) + (int)((int)camera.Position.z / CHUNK_SIZE_Z);
	size_t o = 0;

	if (vox->chunkCount < 256)
	{
		for (int x = (-VIEW_DISTANCE) + (int)((int)camera.Position.x / CHUNK_SIZE_X); x < new_view_distance_x; x++)
		{
			for (int z = (-VIEW_DISTANCE) + (int)((int)camera.Position.z / CHUNK_SIZE_Z); z < new_view_distance_z; z++)
			{
				if (createExpendedChunkX(vox, chunks, x, z, o))
				{
					//goto stop;
					o++;
				}
			}
		}
	}
	//stop:;
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
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		camera.MovementSpeed = 50;
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE)
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
