#include <filesystem>
#include "../headers/vox.hpp"

Camera camera(glm::vec3(0.0f, 128.0f, 0.0f));
Frustum frustum;
std::unordered_map<glm::vec3, unsigned int, MyHashFunction> renderedKeys;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

std::mutex mtx;
std::mutex mtxb;
std::mutex mtxc;
std::mutex mtxd;

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
	vox->seed = 0; //rand() % 10000;
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

int createExpendedChunkX(t_vox *vox, ChunkMap *chunks, int x, int z, int o)
{
	mtxd.lock();
	//printf("%d\n", renderedKeys.size());
	mtx.lock();
	if (renderedKeys.find(glm::vec3(x, 0, z)) == renderedKeys.end() && chunks->find(glm::vec3(x, 0, z)) == chunks->end())
	{
		mtxd.unlock();
		chunks->insert(PosChunk(glm::vec3(x, 0, z), new Chunk(createCube(vox, o, glm::vec3(x * CHUNK_SIZE_X, 0, z * CHUNK_SIZE_Z), vox->seed))));
		mtx.unlock();
		return 1;
	}
	mtxd.unlock();
	mtx.unlock();
	return 0;
}
void chunkThread(ChunkMap *chunks, t_vox *vox, int start_x, int end_x, int start_z, int end_z)
{
	while (1)
	{
		createChunk(vox, chunks, start_x, end_x, start_z, end_z);
	}
}

void meshThread(ChunkMap *chunks, MeshMap *mesh)
{
	while (1)
	{
		std::vector<glm::vec3> vec;
		mtx.lock();
		ChunkMap chunkTmp = *chunks;
		mtx.unlock();
		for (auto c = chunkTmp.begin(); c != chunkTmp.end(); ++c)
		{
			mtxc.lock();
			if (mesh->find(c->first) == mesh->end())
			{
				mesh->emplace(PosMesh((c->first), new Mesh(createMesh(*c->second))));
				mtxc.unlock();
				break;
			}
			//else if (mesh->find(c->first)->second->rendered == true)
			//{
			//	vec.emplace_back(c->first);
			//	//break;
			//}
			mtxc.unlock();
		}
		mtxc.unlock();
		mtxd.lock();
		for (auto &&key : renderedKeys)
		{
			//printf("%d\n", chunks->find(key)->second->Id);
			mtx.lock();
			mtxc.lock();
			//printf("%d\n", chunks->size());
			//delete (chunks->find(key)->second);
			//chunks->find(key)->second->freeCubeData();
			chunks->erase(key.first);
			//delete (mesh->find(key)->second);
			mesh->erase(key.first);
			mtxc.unlock();
			mtx.unlock();
			if (key.second)
				renderedKeys.erase(key.first);
			//break;
		}
		mtxd.unlock();
	}
}

int main(void)
{

	GLFWwindow *window;
	ChunkMap chunks;
	BufferMap buffer;
	MeshMap mesh;

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

	Cubemap skybox;

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	Shader shader("./shaders/vertex.glsl", "./shaders/fragment.glsl");
	Shader skyboxShader("./shaders/skyboxVs.glsl", "./shaders/skyboxFs.glsl");
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window, key_callback);

	shader.use();
	load_texture("./resources/textures/textures.png");
	shader.setInt("texture", 0);
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//std::thread update_thread(update, &chunks, vox);
	//std::thread update_thread2(update2, &chunks, vox);
	std::vector<std::thread> threads(2);
	//threads[0] = std::thread(update, &chunks, vox, -VIEW_DISTANCE, VIEW_DISTANCE, -VIEW_DISTANCE, VIEW_DISTANCE);
	threads[0] = std::thread(chunkThread, &chunks, vox, -VIEW_DISTANCE, VIEW_DISTANCE, -VIEW_DISTANCE, VIEW_DISTANCE);
	threads[1] = std::thread(meshThread, &chunks, &mesh);
	//threads[2] = std::thread(update, &chunks, vox, -VIEW_DISTANCE, 0, 0, VIEW_DISTANCE);
	//threads[3] = std::thread(update, &chunks, vox, 0, VIEW_DISTANCE, -VIEW_DISTANCE, 0);

	while (!glfwWindowShouldClose(window))
	{
		//fps(window);
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.69f, 0.94f, 0.95f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, VIEW_DISTANCE * VIEW_DISTANCE + CHUNK_SIZE_X * 4);
		shader.setMat4("projection", projection);
		glm::mat4 view = camera.GetViewMatrix();
		shader.setMat4("view", view);
		frustum.Transform(projection, view);
		shader.setVec3("lightPos", glm::vec3(0.7, 0.2, 0.5));
		shader.setVec3("viewPos", camera.Position);
		displayChunk(&buffer, &mesh, shader);
		displayVAO(&buffer, shader);

		glDepthFunc(GL_LEQUAL); // change depth function so depth test passes when values are equal to depth buffer's content
		skyboxShader.use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
		skyboxShader.setMat4("view", view);
		skyboxShader.setMat4("projection", projection);
		// skybox cube
		glBindVertexArray(skybox.skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);

	glfwTerminate();
	exit(EXIT_SUCCESS);
}

Mesh createMesh(Chunk chunk)
{
	Mesh mesh(chunk.Position);
	glm::vec2 tex = glm::vec2(0, 0);
	for (size_t x = 0; x < CHUNK_SIZE_X; x++)
	{
		for (size_t y = 0; y < CHUNK_SIZE_Y; y++)
		{
			for (size_t z = 0; z < CHUNK_SIZE_Z; z++)
			{
				if (chunk.CubeData[x][y][z].isEmpty)
					continue;
				for (int i = 0; i < 6; i++)
				{
					tex = chunk.CubeData[x][y][z].texCoord;
					if (tex.x == 0 && tex.y == 0 && i == 4)
						tex = glm::vec2(1, 0);
					if (tex.x == 0 && tex.y == 0 && i == 5)
						tex = glm::vec2(2, 0);
					if (!mesh.getNeighbor(x, y, z, (Direction)i, chunk.CubeData) || chunk.CubeData[x][y][z].texCoord == glm::vec2(2, 3))
					{
						for (size_t j = 0; j < 18; j += 3)
						{
							mesh.Vertices.push_back(glm::vec3(VERTICES[j + (i * 18)] + chunk.CubeData[x][y][z].Position.x,
															  VERTICES[j + 1 + (i * 18)] + chunk.CubeData[x][y][z].Position.y,
															  VERTICES[j + 2 + (i * 18)] + chunk.CubeData[x][y][z].Position.z));

							mesh.Normal.push_back(glm::vec3(NORMAL[j + (i * 18)],
															NORMAL[j + 1 + (i * 18)],
															NORMAL[j + 2 + (i * 18)]));
							mesh.texCoord.push_back(tex);
							mesh.size += 3;
						}
						for (size_t k = 0; k < 12; k += 2)
						{
							mesh.UV.push_back(glm::vec2(UV[k + (i * 12)],
														UV[k + 1 + (i * 12)]));
						}
					}
				}
			}
		}
	}
	mesh.maxHeight = chunk.maxHeight;
	return mesh;
}

float heightSimplex(float n, float a, float freq, int x, int z, float seed, glm::vec3 offsets)
{
	SimplexNoise noise;
	//float n = 0.5;
	//float a = 0.3;
	//float freq = 0.0032;
	float value = 0;
	for (int octave = 0; octave < 8; octave++)
	{
		value = a * noise.noise((x + offsets.x + (float)seed) * freq, (z + offsets.z + (float)seed) * freq, 1);
		n += value;
		a *= 0.5;
		freq *= 2.0;
	}
	n = (n + 1) / 2;
	n *= CHUNK_SIZE_Y / 2;

	return n;
}

float desertSimplex(int x, int z, float seed, glm::vec3 offsets)
{
	SimplexNoise noise;
	float n = 0.3;
	float a = 2.2;
	float freq = 0.0008;
	float value = 3;
	for (int octave = 0; octave < 8; octave++)
	{
		value = a * noise.noise((x + offsets.x + (float)seed) * freq, (z + offsets.z + (float)seed) * freq, 1);
		n += value;
		a *= 0.5;
		freq *= 2.0;
	}
	n = (n + 1) / 2;

	return n;
}

float caveSimplex(int x, int y, int z, float seed, glm::vec3 offsets)
{
	SimplexNoise noise;
	float n = 0.5;
	float a = 0.7;
	float freq = 0.0632;
	float value = 0;
	for (int octave = 0; octave < 2; octave++)
	{
		value = a * noise.noise((x + offsets.x + (float)seed) * freq, (y + (float)seed) * freq, (z + offsets.z + (float)seed) * freq);
		n += value;
		a *= 0.5;
		freq *= 2.0;
	}
	return n;
}

void populateChunk(Chunk *chunk, int x, int n, int z, int id, float b, int maxHeight)
{
	chunk->CubeData[x][(int)n][z].Position = glm::vec3((x), (int)(n), (z));
	chunk->CubeData[x][(int)n][z].Id = id;
	chunk->CubeData[x][(int)n][z].texCoord = selectTex(n, b);
	chunk->CubeData[x][(int)n][z].isEmpty = false;
	chunk->CubeData[x][(int)n][z].value = n;
	chunk->maxHeight = maxHeight;
}

void createTree(Chunk *chunk, int x, int n, int z, int id)
{
	for (size_t i = 0; i < 5; i++)
	{
		chunk->CubeData[x][(int)n + i][z].Position = glm::vec3((x), (int)(n + i), (z));
		chunk->CubeData[x][(int)n + i][z].texCoord = glm::vec2(0, 3);
		chunk->CubeData[x][(int)n + i][z].isEmpty = false;
	}
	for (size_t j = 0; j < 3; j++)
	{
		for (size_t k = 0; k < 3; k++)
		{
			for (size_t l = 0; l < 3; l++)
			{
				if (x - 1 + k > 0 && x - 1 + k < 16 && n + 3 + l > 0 && n + 3 + l < 256 && z - 1 + j > 0 && z - 1 + j < 16)
				{
					chunk->CubeData[x - 1 + k][(int)n + 3 + l][z - 1 + j].Position = glm::vec3((x - 1 + k), (int)(n + 3 + l), (z - 1 + j));
					chunk->CubeData[x - 1 + k][(int)n + 3 + l][z - 1 + j].texCoord = glm::vec2(2, 3);
					chunk->CubeData[x - 1 + k][(int)n + 3 + l][z - 1 + j].isEmpty = false;
					chunk->maxHeight = (int)n + 3 + l;
				}
			}
		}
	}
}

Chunk createCube(t_vox *vox, int chunkId, glm::vec3 offsets, int seed)
{
	int id = 0;
	SimplexNoise noise;
	Chunk chunk(offsets, chunkId);
	for (size_t x = 0; x < CHUNK_SIZE_X; x++)
	{
		for (size_t z = 0; z < CHUNK_SIZE_Z; z++)
		{
			float n = 0;
			int maxHeight = 0;
			float b = desertSimplex(x, z, seed, offsets);
			if (b > 1.2)
				n = heightSimplex(0.5, 0.3, 0.0072, x, z, seed, offsets);
			else
				n = heightSimplex(0.5, 0.3, 0.0032, x, z, seed, offsets);

			if (n <= CHUNK_SIZE_Y / 3)
				n = CHUNK_SIZE_Y / 3;

			if (n > maxHeight)
				maxHeight = n;
			float r = noise.noise(x + offsets.x, z + offsets.z);
			r = (r + 1) / 2;
			//printf("%f\n", r);
			populateChunk(&chunk, x, n, z, id, b, maxHeight);

			if (n > CHUNK_SIZE_Y / 3 + 4 && b < 0.9)
			{
				if (r > 0.9835)
				{
					createTree(&chunk, x, n, z, id);
				}
			}

			id = id + 1 + chunkId * (vox->chunkNbX + vox->chunkNbZ);

			for (int y = 50; y < (int)n; y++)
			{
				float nn = caveSimplex(x, y, z, seed, offsets);

				nn = (nn + 1) / 2;
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
	return chunk;
}

glm::vec2 selectTex(float n, float b)
{
	if (n < CHUNK_SIZE_Y / 3)
		return glm::vec2(0, 1);
	if (n < CHUNK_SIZE_Y / 3 + 3)
		return glm::vec2(1, 1);
	if (b > 0.9 && b < 1.2)
		return glm::vec2(1, 1);
	if (b > 1.2)
		return glm::vec2(0, 2);
	return glm::vec2(0, 0);
}

//bool comp(const std::pair<glm::vec3, Chunk *> &c1, const std::pair<glm::vec3, Chunk *> &c2)
//{
//	int distanceFromChunk1 = sqrt(pow((camera.Position.x - (c1.second->Position.x + 8)), 2) + pow((camera.Position.z - (c1.second->Position.z + 8)), 2));
//	int distanceFromChunk2 = sqrt(pow((camera.Position.x - (c2.second->Position.x + 8)), 2) + pow((camera.Position.z - (c2.second->Position.z + 8)), 2));
//	return distanceFromChunk1 < distanceFromChunk2;
//}

void displayChunk(BufferMap *buffer, MeshMap *mesh, Shader shader)
{
	int triNb = 0;
	mtxc.lock();
	MeshMap meshTmp = *mesh;
	mtxc.unlock();

	if (meshTmp.size() > 0)
	{
		for (auto it = meshTmp.begin(); it != meshTmp.end(); ++it)
		{
			if (it->second->rendered == false)
			{
				buffer->insert(PosBuffer(it->first, new Buffer()));
				buffer->find(it->first)->second->loadVBO(*it->second);
				buffer->find(it->first)->second->mat = it->second->mat;
				buffer->find(it->first)->second->maxHeight = it->second->maxHeight;
				buffer->find(it->first)->second->size = it->second->size;
				buffer->find(it->first)->second->Position = it->second->Position;
				mtxc.lock();
				mesh->find(it->first)->second->rendered = true;
				mesh->find(it->first)->second->Vertices.clear();
				mesh->find(it->first)->second->Vertices.shrink_to_fit();
				mesh->find(it->first)->second->UV.clear();
				mesh->find(it->first)->second->UV.shrink_to_fit();
				mesh->find(it->first)->second->texCoord.clear();
				mesh->find(it->first)->second->texCoord.shrink_to_fit();
				mesh->find(it->first)->second->Normal.clear();
				mesh->find(it->first)->second->Normal.shrink_to_fit();
				mtxc.unlock();
				mtxd.lock();
				renderedKeys.insert(PosRendered(it->first, 0));
				mtxd.unlock();
				return;
			}
		}
		//printf("%d\n", triNb);
	}
}

void displayVAO(BufferMap *buffer, Shader shader)
{
	std::vector<glm::vec3> vec;
	//std::vector<std::pair<glm::vec3, Chunk *>> elems(chunks->begin(), chunks->end());
	//std::sort(elems.begin(), elems.end(), comp);
	//mtxb.lock();
	for (auto it = buffer->begin(); it != buffer->end(); ++it)
	{
		int distanceFromChunk = sqrt(pow((camera.Position.x - (it->second->Position.x + 8)), 2) + pow((camera.Position.z - (it->second->Position.z + 8)), 2));

		if (distanceFromChunk > VIEW_DISTANCE * VIEW_DISTANCE * 1.4)
		{
			vec.emplace_back(it->first);
			continue;
		}
		else if (distanceFromChunk > VIEW_DISTANCE * VIEW_DISTANCE || frustum.IsInside(glm::vec3(it->second->Position.x, it->second->maxHeight, it->second->Position.z)) == Frustum::Invisible)
		{
			continue;
		}
		else
		{
			shader.setMat4("model", it->second->mat);
			glBindVertexArray(it->second->VAO);
			glDrawArrays(GL_TRIANGLES, 0, it->second->size / 3);
			//triNb += it->second->size / 3 / 3;
		}
	}
	for (auto &&key : vec)
	{
		//mtxb.lock();
		delete (buffer->find(key)->second);
		buffer->erase(key);
		mtxd.lock();
		renderedKeys.find(key)->second = 1;
		mtxd.unlock();
		//mtxb.unlock();
	}
	//mtxb.unlock();
}

void createChunk(t_vox *vox, ChunkMap *chunks, int start_x, int end_x, int start_z, int end_z)
{
	int new_view_distance_x = ((end_x) + (int)((int)camera.Position.x / CHUNK_SIZE_X));
	int new_view_distance_z = ((end_z) + (int)((int)camera.Position.z / CHUNK_SIZE_Z));
	size_t o = 0;
	for (int x = (start_x) + (int)((int)camera.Position.x / CHUNK_SIZE_X); x < new_view_distance_x; x++)
	{
		for (int z = (start_z) + (int)((int)camera.Position.z / CHUNK_SIZE_Z); z < new_view_distance_z; z++)
		{
			Frustum::Visibility visiblity = frustum.IsInside(glm::vec3(x * CHUNK_SIZE_X, 256, z * CHUNK_SIZE_Z));
			if (visiblity == Frustum::Partially || visiblity == Frustum::Completly)
			{
				if (createExpendedChunkX(vox, chunks, x, z, o))
					return;
			}
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
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	return texture1;
}
