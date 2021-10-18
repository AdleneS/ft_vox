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
	FT_Library ft;
	GLuint fontVAO;
	GLuint fontVBO;
	GLuint texture1;
	GLFWmonitor *primary;
	srand((unsigned int)time(NULL));
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::shuffle(perm.begin(), perm.end(), std::default_random_engine(seed));

	if (!glfwInit())
		exit(EXIT_FAILURE);
	glfwSetErrorCallback(error_callback);
	primary = glfwGetPrimaryMonitor();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	const GLFWvidmode *mode = glfwGetVideoMode(primary);
	window = glfwCreateWindow(mode->width, mode->height, "ft_vox", primary, NULL);

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
	Shader textShader("./shaders/textVs.glsl", "./shaders/textFs.glsl");
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window, key_callback);

	shader.use();
	texture1 = load_texture("./resources/textures/textures.png");
	shader.setInt("texture1", 0);
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));
	textShader.use();
	glUniformMatrix4fv(glGetUniformLocation(textShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	initFreeType(&ft, &fontVAO, &fontVBO);

	std::vector<std::thread> threads(5);
	threads[0] = std::thread(chunkThread, &chunks, window, -VIEW_DISTANCE, 0, -VIEW_DISTANCE, 0);
	threads[1] = std::thread(chunkThread, &chunks, window, -VIEW_DISTANCE, 0, 0, VIEW_DISTANCE);
	threads[2] = std::thread(chunkThread, &chunks, window, 0, VIEW_DISTANCE, 0, VIEW_DISTANCE);
	threads[3] = std::thread(chunkThread, &chunks, window, 0, VIEW_DISTANCE, -VIEW_DISTANCE, 0);
	threads[4] = std::thread(meshThread, &chunks, window, &mesh);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.69f, 0.94f, 0.95f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
		shader.setMat4("projection", projection);
		glm::mat4 view = camera.GetViewMatrix();
		shader.setMat4("view", view);
		frustum.Transform(projection, view);
		shader.setVec3("lightPos", glm::vec3(0.7, 0.2, 0.5));
		shader.setVec3("viewPos", camera.Position);
		shader.setBool("light", light);
		mtxc.lock();
		bufferGeneration(&buffer, &mesh, shader);
		mtxc.unlock();
		displayVAO(&buffer, shader);
		skyboxDisplay(skyboxShader, skybox, projection);

		RenderText(textShader, 0, SCR_HEIGHT - 50, 1.0f, glm::vec3(0.5, 0.8f, 0.2f), fontVAO, fontVBO);
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

// bool comp(const std::pair<glm::vec3, Chunk *> &c1, const std::pair<glm::vec3, Chunk *> &c2)
//{
//	int distanceFromChunk1 = sqrt(pow((camera.Position.x - (c1.second->Position.x + 8)), 2) + pow((camera.Position.z - (c1.second->Position.z + 8)), 2));
//	int distanceFromChunk2 = sqrt(pow((camera.Position.x - (c2.second->Position.x + 8)), 2) + pow((camera.Position.z - (c2.second->Position.z + 8)), 2));
//	return distanceFromChunk1 < distanceFromChunk2;
// }

void displayVAO(BufferMap *buffer, Shader shader)
{
	std::vector<glm::vec3> vec;
	// std::vector<std::pair<glm::vec3, Chunk *>> elems(chunks->begin(), chunks->end());
	// std::sort(elems.begin(), elems.end(), comp);
	// printf("%d\n", buffer->size());
	for (auto it = buffer->begin(); it != buffer->end(); ++it)
	{
		int distanceFromChunk = sqrt(pow((camera.Position.x - (it->second->Position.x + CHUNK_SIZE_X / 2)), 2) + pow((camera.Position.z - (it->second->Position.z + CHUNK_SIZE_Z / 2)), 2));

		if (distanceFromChunk > VIEW_DISTANCE * VIEW_DISTANCE + (CHUNK_SIZE_X + CHUNK_SIZE_Y))
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
			// triNb += it->second->size / 3 / 3;
		}
	}

	// printf("%d\n", triNb);
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
	static int oldState = GLFW_PRESS;
	int newState = glfwGetKey(window, GLFW_KEY_E);
	if (newState == GLFW_PRESS && oldState == GLFW_RELEASE)
	{
		light = light ? false : true;
	}
	oldState = newState;
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

int initFreeType(FT_Library *ft, GLuint *VAO, GLuint *VBO)
{

	if (FT_Init_FreeType(&*(ft)))
	{
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		return -1;
	}

	// find path to font
	std::string font_name = "./resources/font/arial.ttf";
	if (font_name.empty())
	{
		std::cout << "ERROR::FREETYPE: Failed to load font_name" << std::endl;
		return -1;
	}

	// load font as face
	FT_Face face;
	if (FT_New_Face(*ft, font_name.c_str(), 0, &face))
	{
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		return -1;
	}
	else
	{
		// set size to load glyphs as
		FT_Set_Pixel_Sizes(face, 0, 48);

		// disable byte-alignment restriction
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// load first 128 characters of ASCII set
		for (unsigned char c = 0; c < 128; c++)
		{
			// Load character glyph
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
				continue;
			}
			// generate texture
			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer);
			// set texture options
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// now store character for later use
			Character character = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				static_cast<unsigned int>(face->glyph->advance.x)};
			Characters.insert(std::pair<char, Character>(c, character));
		}
		// glBindTexture(GL_TEXTURE_2D, 0);
	}
	// destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(*ft);

	// configure VAO/VBO for texture quads
	// ----------------------------------
	glActiveTexture(GL_TEXTURE1);
	glGenVertexArrays(1, &(*VAO));
	glGenBuffers(1, &(*VBO));
	glBindVertexArray(*VAO);
	glBindBuffer(GL_ARRAY_BUFFER, *VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	return 1;
}

void RenderText(Shader &textShader, float x, float y, float scale, glm::vec3 color, GLuint VAO, GLuint VBO)
{
	static double current_time = 0;
	static double last_time = 0;
	last_time = current_time;
	current_time = glfwGetTime();
	double fps = 1.0 / (current_time - last_time);
	std::ostringstream ss;
	ss << fps;
	std::string text(ss.str());

	textShader.use();
	glUniform3f(glGetUniformLocation(textShader.ID, "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		float xpos = x + ch.Bearing.x * scale;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;

		float vertices[6][4] = {
			{xpos, ypos + h, 0.0f, 0.0f},
			{xpos, ypos, 0.0f, 1.0f},
			{xpos + w, ypos, 1.0f, 1.0f},

			{xpos, ypos + h, 0.0f, 0.0f},
			{xpos + w, ypos, 1.0f, 1.0f},
			{xpos + w, ypos + h, 1.0f, 0.0f}};

		glBindTexture(GL_TEXTURE_2D, ch.TextureID);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);

		x += (ch.Advance >> 6) * scale;
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}