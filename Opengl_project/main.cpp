#include <windows.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"
#include "Shader.h"
#include "Camera.h"
#include "texture_loader.h"
#include "vertex_loader.h"
#include "Text_render.h"
#include "Helper.h"
#include "Model.h"

#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void APIENTRY glDebugOutput(GLenum source,
							GLenum type,
							GLuint id,
							GLenum severity,
							GLsizei length,
							const GLchar *message,
							const void *userParam);

unsigned int loadCubemap(std::vector<std::string> faces, std::string fc_dir);
unsigned int createCubeVAO(std::string path);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const float BOOST_MAX = 0.15f;

GLFWwindow* window;
fps_cam::Camera myCamera;
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
Shader *textShader;
Shader *modelShader;
Shader *skyboxShader;
clock_t last_fps_tacts;
GLuint fps;
GLuint frames;
mdl::Model *nanoSuitModel;
unsigned int cubeMap;
unsigned int cubeVAO;

float delta_time = 0.0f;
float last_frame = 0.0f;
float current_frame = 0.0f;

bool cursor_disabled = true;

bool init()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	glEnable(GL_DEPTH_TEST);

	GLint flags = 0;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}

	tsp::init("c:\\Users\\fghft\\source\\repos\\Opengl_project\\Fonts\\ariali.ttf", 48, SCR_WIDTH, SCR_HEIGHT);


	return true;
}


void drawScene()
{
	model = glm::mat4(1.0f);
	view = myCamera.getViewMat();
	projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
	glActiveTexture(GL_TEXTURE1);
	modelShader->use();
	modelShader->setUniformMat4("model", model);
	modelShader->setUniformMat4("view", view);
	modelShader->setUniformMat4("projection", projection);
	modelShader->setUniformVec3("camPos", myCamera.getPos());

	nanoSuitModel->draw(*modelShader);

	glDepthFunc(GL_LEQUAL);
	skyboxShader->use();
	skyboxShader->setUniformMat4("projection", projection);
	skyboxShader->setUniformMat4("view", glm::mat4(glm::mat3(view)));
	glBindVertexArray(cubeVAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthFunc(GL_LESS);
}

void draw_inf()
{
	tsp::render_text(*textShader, "fps:" + help::itos(fps), 0.0f, 775.0f, 0.4f, glm::vec3(1.0f, 1.0f, 1.0f));

	glm::vec3 cPos = myCamera.getPos();

	std::string cords = "x:" + help::itos((int)cPos.x) + " y:" + help::itos((int)cPos.y) + " z:" + help::itos((int)cPos.z);

	tsp::render_text(*textShader, cords, 1.0f, 3.0f, 0.4f, glm::vec3(1.0f, 1.0f, 1.0f));
}

void draw()
{
	drawScene();
	draw_inf();
}



void calc_fps()
{
	clock_t cur_tacts = std::clock();
	time_t delta_tacts = cur_tacts - last_fps_tacts;

	if ((double)delta_tacts / (double)CLOCKS_PER_SEC >= 1.0)
	{
		fps = (double)frames / ((double)delta_tacts / CLOCKS_PER_SEC);
		last_fps_tacts = std::clock();
		frames = 0;
	}
}

int main()
{
	if (!init())
		return -1;


	textShader = new Shader("c:\\Users\\fghft\\source\\repos\\Opengl_project\\Shaders\\text.vs", "c:\\Users\\fghft\\source\\repos\\Opengl_project\\Shaders\\text.fs");
	modelShader = new Shader("c:\\Users\\fghft\\source\\repos\\Opengl_project\\Shaders\\modelShader.vert", "c:\\Users\\fghft\\source\\repos\\Opengl_project\\Shaders\\modelShader.frag");
	skyboxShader = new Shader("c:\\Users\\fghft\\source\\repos\\Opengl_project\\Shaders\\skyboxShader.vert", "c:\\Users\\fghft\\source\\repos\\Opengl_project\\Shaders\\skyboxShader.frag");

	nanoSuitModel = new mdl::Model("c:\\Users\\fghft\\source\\repos\\Opengl_project\\Models\\Nano_suit\\scene.fbx");

	skyboxShader->use();
	skyboxShader->setUniformValue("cubemap", 1);

	std::vector<std::string> faces = 
	{
		"right.jpg",
		"left.jpg",
		"top.jpg",
		"bottom.jpg",
		"front.jpg",
		"back.jpg"
	};

	std::string fc_dir = "c://Users//fghft//source//repos//Opengl_project//textures//skybox//";

	cubeMap = loadCubemap(faces, fc_dir);
	cubeVAO = createCubeVAO("c://Users//fghft//source//repos//Opengl_project//verticies//skybox.txt");

	last_frame = glfwGetTime();
	last_fps_tacts = std::clock();

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		calc_fps();
		draw();

		current_frame = glfwGetTime();
		delta_time = current_frame - last_frame;
		last_frame = current_frame;

		glfwSwapBuffers(window);
		glfwPollEvents();

		++frames;
	}

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	static bool boost = false;
	static int pressed = 0;
	static int second_press = 0;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		if (second_press == 0)
			++second_press;


		++pressed;

		if (pressed > 2)
			pressed = 2;

		float current_foward = glfwGetTime();
		static float last_foward = current_foward;
		float foward_delta_time = current_foward - last_foward;
		
		last_foward = current_foward;

		if (foward_delta_time <= BOOST_MAX && second_press == 2)
		{
			myCamera.processKeyboard(fps_cam::Movement::forward, delta_time, fps_cam::SPEED * 3.0f);
			boost = true;
		}
		else if(boost == true && pressed == 2)
		{
			myCamera.processKeyboard(fps_cam::Movement::forward, delta_time, fps_cam::SPEED * 3.0f);
		}
		else
		{
			boost = false;
			pressed = 0;
			myCamera.processKeyboard(fps_cam::Movement::forward, delta_time);
		}

		if (second_press == 2)
			second_press = 0;
	}
	else
	{
		pressed = 0;

		if (second_press == 1)
			++second_press;
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		myCamera.processKeyboard(fps_cam::Movement::left, delta_time);

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		myCamera.processKeyboard(fps_cam::Movement::back, delta_time);

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		myCamera.processKeyboard(fps_cam::Movement::right, delta_time);

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		myCamera.addY(delta_time);

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		myCamera.addY(-delta_time);

	if(glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		if (cursor_disabled)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			cursor_disabled = false;
		}
		else
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			cursor_disabled = true;
		}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xPos, double yPos)
{
	static bool first_enter = true;

	static float lastX;
	static float lastY;

	if (first_enter)
	{
		lastX = xPos;
		lastY = yPos;
		first_enter = false;
	}

	float xoffset = xPos - lastX;
	float yoffset = lastY - yPos;

	lastX = xPos;
	lastY = yPos;

	myCamera.processMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	myCamera.processMouseScroll(yoffset);
}


void APIENTRY glDebugOutput(GLenum source,
							GLenum type,
							GLuint id,
							GLenum severity,
							GLsizei length,
							const GLchar *message,
							const void *userParam)
{
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

	std::cout << "---------------" << std::endl;
	std::cout << "Debug message (" << id << "): " << message << std::endl;

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
	case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
	} std::cout << std::endl;

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
	case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
	} std::cout << std::endl;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
	case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
	} std::cout << std::endl;

	std::cout << std::endl;
}



unsigned int loadCubemap(std::vector<std::string> faces, std::string fc_dir)
{
	unsigned int textureID;

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	unsigned char *data;
	int width, height, channels;

	stbi_set_flip_vertically_on_load(false);

	for (int i = 0; i < faces.size(); ++i)
	{
		data = stbi_load((fc_dir + faces[i]).c_str(), &width, &height, &channels, 0);

		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
		}

		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

unsigned int createCubeVAO(std::string path)
{
	unsigned int VAO, VBO;

	std::vector<float> data;

	load_verticies(path.c_str(), data);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data.size(), &data[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	return VAO;
}