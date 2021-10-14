#include <filesystem>
#include "../headers/vox.hpp"

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
	ChunkMap chunks;
	BufferMap buffer;
	MeshMap mesh;

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

	std::vector<std::thread> threads(5);
	threads[0] = std::thread(chunkThread, &chunks, -VIEW_DISTANCE, 0, -VIEW_DISTANCE, 0);
	threads[1] = std::thread(chunkThread, &chunks, -VIEW_DISTANCE, 0, 0, VIEW_DISTANCE);
	threads[2] = std::thread(chunkThread, &chunks, 0, VIEW_DISTANCE, 0, VIEW_DISTANCE);
	threads[3] = std::thread(chunkThread, &chunks, 0, VIEW_DISTANCE, -VIEW_DISTANCE, 0);
	threads[4] = std::thread(meshThread, &chunks, &mesh);

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
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
		shader.setMat4("projection", projection);
		glm::mat4 view = camera.GetViewMatrix();
		shader.setMat4("view", view);
		frustum.Transform(projection, view);
		shader.setVec3("lightPos", glm::vec3(0.7, 0.2, 0.5));
		shader.setVec3("viewPos", camera.Position);
		mtxc.lock();
		bufferGeneration(&buffer, &mesh, shader);
		mtxc.unlock();
		displayVAO(&buffer, shader);
		skyboxDisplay(skyboxShader, skybox, projection);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);

	glfwTerminate();
	exit(EXIT_SUCCESS);
}

void skyboxDisplay(Shader skyboxShader, Cubemap skybox, glm::mat4 projection)
{
	glDepthFunc(GL_LEQUAL);
	skyboxShader.use();
	glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
	skyboxShader.setMat4("view", view);
	skyboxShader.setMat4("projection", projection);
	glBindVertexArray(skybox.skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
}

//bool comp(const std::pair<glm::vec3, Chunk *> &c1, const std::pair<glm::vec3, Chunk *> &c2)
//{
//	int distanceFromChunk1 = sqrt(pow((camera.Position.x - (c1.second->Position.x + 8)), 2) + pow((camera.Position.z - (c1.second->Position.z + 8)), 2));
//	int distanceFromChunk2 = sqrt(pow((camera.Position.x - (c2.second->Position.x + 8)), 2) + pow((camera.Position.z - (c2.second->Position.z + 8)), 2));
//	return distanceFromChunk1 < distanceFromChunk2;
//}

void displayVAO(BufferMap *buffer, Shader shader)
{
	std::vector<glm::vec3> vec;
	//std::vector<std::pair<glm::vec3, Chunk *>> elems(chunks->begin(), chunks->end());
	//std::sort(elems.begin(), elems.end(), comp);
	//printf("%d\n", buffer->size());
	for (auto it = buffer->begin(); it != buffer->end(); ++it)
	{
		int distanceFromChunk = sqrt(pow((camera.Position.x - (it->second->Position.x + CHUNK_SIZE_X / 2)), 2) + pow((camera.Position.z - (it->second->Position.z + CHUNK_SIZE_Z / 2)), 2));

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
	//printf("%d\n", triNb);
	for (auto &&key : vec)
	{
		delete (buffer->find(key)->second);
		buffer->erase(key);
		mtxd.lock();
		renderedKeys.erase(key);
		mtxd.unlock();
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