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

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const float BOOST_MAX = 0.15f;

GLFWwindow* window;
fps_cam::Camera myCamera;
Texture *tex1;
Texture *tex2;
Texture *container2_tex;
Texture *container2_specular_tex;
Texture *container2_emission_map;
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
Shader *ourShader;
Shader *textShader;
Shader *lightShader;
Shader *lampShader;
Shader *fbShader;
unsigned int VBO, VAO, lightVAO, lightVBO, lampVAO, FBO, RBO, CBO, fbVAO, fbVBO;
clock_t last_fps_tacts;
GLuint fps;
GLuint frames;
glm::vec3 lightPos(1.2f, 0.2f, 2.0f);
mdl::Model *nanoSuitModel;

float cur_attitude = 0.2;

float delta_time = 0.0f;
float last_frame = 0.0f;
float current_frame = 0.0f;

bool cursor_disabled = true;

const size_t lamps_cnt = 4;

glm::vec3 cubePositions[] = {
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(2.0f,  5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f,  2.0f, -2.5f),
	glm::vec3(1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f)
};

glm::vec3 PointLightsPositions[lamps_cnt] = {
	glm::vec3(-8.0f,  -11.0f,  4.0f),
	glm::vec3(5.0f,  -11.0f,  4.0f),
	glm::vec3(5.0f,  -11.0f,  -3.0f),
	glm::vec3(-8.0f,  -11.0f,  -3.0f)
};

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

void drawLamp(const glm::vec3 &pos)
{
	model = glm::mat4(1.0f);
	model = glm::translate(model, pos);
	model = glm::scale(model, glm::vec3(0.2f));

	lampShader->setUniformMat4("model", model);
	glBindVertexArray(lampVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void drawScene()
{
	view = myCamera.getViewMat();
	projection = glm::perspective(glm::radians(myCamera.getFov()), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);

	lampShader->use();
	lampShader->setUniformMat4("view", view);
	lampShader->setUniformMat4("projection", projection);

	for (int i = 0; i < lamps_cnt; ++i)
		drawLamp(PointLightsPositions[i]);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.0f, -20.0f, 0.0f));
	model = glm::scale(model, glm::vec3(4.0f));

	lightShader->use();
	lightShader->setUniformMat4("view", view);
	lightShader->setUniformMat4("projection", projection);
	lightShader->setUniformMat4("model", model);
	lightShader->setUniformVec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
	lightShader->setUniformVec3("spotLight.position", myCamera.getPos());
	lightShader->setUniformVec3("spotLight.direction", myCamera.getDir());
	lightShader->setUniformVec3("spotLight.diffuse", glm::vec3(0.5f));
	lightShader->setUniformVec3("viewPos", myCamera.getPos());

	for (int i = 0; i < lamps_cnt; ++i)
	{
		static std::string pref = "pointLights[";
		std::string nm = std::to_string(i);

		lightShader->setUniformVec3((pref + nm + std::string("].position")).c_str(), PointLightsPositions[i]);
	}

	nanoSuitModel->draw(*lightShader);
}

void draw_inf();

void draw()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawScene();
	draw_inf();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	fbShader->use();
	glBindVertexArray(fbVAO);
	glDisable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, CBO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void draw_inf()
{
	tsp::render_text(*textShader, "fps:" + help::itos(fps), 0.0f, 775.0f, 0.4f, glm::vec3(1.0f, 1.0f, 1.0f));

	glm::vec3 cPos = myCamera.getPos();

	std::string cords = "x:" + help::itos((int)cPos.x) + " y:" + help::itos((int)cPos.y) + " z:" + help::itos((int)cPos.z);

	tsp::render_text(*textShader, cords, 1.0f, 3.0f, 0.4f, glm::vec3(1.0f, 1.0f, 1.0f));

	//tsp::render_text(*textShader, "+", 290.0f, 390.0f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
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

	ourShader = new Shader("c:\\Users\\fghft\\source\\repos\\Opengl_project\\Shaders\\triangle.vs", "c:\\Users\\fghft\\source\\repos\\Opengl_project\\Shaders\\triangle1.fs");
	textShader = new Shader("c:\\Users\\fghft\\source\\repos\\Opengl_project\\Shaders\\text.vs", "c:\\Users\\fghft\\source\\repos\\Opengl_project\\Shaders\\text.fs");
	lightShader = new Shader("c:\\Users\\fghft\\source\\repos\\Opengl_project\\Shaders\\light.vs", "c:\\Users\\fghft\\source\\repos\\Opengl_project\\Shaders\\light.fs");
	lampShader = new Shader("c:\\Users\\fghft\\source\\repos\\Opengl_project\\Shaders\\lamp.vs", "c:\\Users\\fghft\\source\\repos\\Opengl_project\\Shaders\\lamp.fs");
	fbShader = new Shader("c:\\Users\\fghft\\source\\repos\\Opengl_project\\Shaders\\frameBuffer.vs", "c:\\Users\\fghft\\source\\repos\\Opengl_project\\Shaders\\frameBuffer.fs");

	nanoSuitModel = new mdl::Model("c:\\Users\\fghft\\source\\repos\\Opengl_project\\Models\\glados\\GLaDOS.fbx");

	std::vector<float> vertices;
	std::vector<float> fbVertices;

	load_verticies("c:\\Users\\fghft\\source\\repos\\Opengl_project\\verticies\\verticies.txt", vertices);
	load_verticies("c:\\Users\\fghft\\source\\repos\\Opengl_project\\verticies\\fbVertices.txt", fbVertices);

	
	//cube
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//frame bufffer

	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &CBO);
	glBindTexture(GL_TEXTURE_2D, CBO);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, CBO, 0);

	glBindTexture(GL_TEXTURE_2D, 0);

	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		return -1;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenVertexArrays(1, &fbVAO);
	glGenBuffers(1, &fbVBO);

	glBindVertexArray(fbVAO);

	glBindBuffer(GL_ARRAY_BUFFER, fbVBO);
	glBufferData(GL_ARRAY_BUFFER, fbVertices.size() * sizeof(float), &fbVertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	fbShader->use();
	fbShader->setUniformValue("screenTexture", 0);

	//light
	std::vector<float> lightVerticies;

	load_verticies("c:\\Users\\fghft\\source\\repos\\Opengl_project\\verticies\\light_verticies.txt", lightVerticies);

	glGenVertexArrays(1, &lightVAO);
	glGenBuffers(1, &lightVBO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
	glBufferData(GL_ARRAY_BUFFER, lightVerticies.size() * sizeof(float), &lightVerticies[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//lamp

	glGenVertexArrays(1, &lampVAO);
	glBindVertexArray(lampVAO);
	glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
	glBufferData(GL_ARRAY_BUFFER, lightVerticies.size() * sizeof(float), &lightVerticies[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//light shader uniforms

	lightShader->use();
	lightShader->setUniformValue("material.shininess", 32.0f);
	lightShader->setUniformVec3("dirLight.ambient", glm::vec3(0.2f));
	lightShader->setUniformVec3("dirLight.diffuse", glm::vec3(0.5f));
	lightShader->setUniformVec3("dirLight.specular", glm::vec3(1.0f));
	lightShader->setUniformVec3("spotLight.ambient", glm::vec3(0.2f));
	lightShader->setUniformVec3("spotLight.diffuse", glm::vec3(0.5f));
	lightShader->setUniformVec3("spotLight.specular", glm::vec3(1.0f));
	lightShader->setUniformValue("spotLight.constant", 1.0f);
	lightShader->setUniformValue("spotLight.linear", 0.09f);
	lightShader->setUniformValue("spotLight.quadratic", 0.032f);
	lightShader->setUniformValue("spotLight.innerCone", glm::cos(glm::radians(12.5f)));
	lightShader->setUniformValue("spotLight.outerCone", glm::cos(glm::radians(17.5f)));


	for (int i = 0; i < lamps_cnt; ++i)
	{
		static std::string pref = "pointLights[";
		std::string nm = std::to_string(i);

		lightShader->setUniformVec3((pref + nm + std::string("].ambient")).c_str(), glm::vec3(0.05f));
		lightShader->setUniformVec3((pref + nm + std::string("].diffuse")).c_str(), glm::vec3(0.8f));
		lightShader->setUniformVec3((pref + nm + std::string("].specular")).c_str(), glm::vec3(1.0f));

		lightShader->setUniformValue((pref + nm + std::string("].constant")).c_str(), 1.0f);
		lightShader->setUniformValue((pref + nm + std::string("].linear")).c_str(), 0.014f);
		lightShader->setUniformValue((pref + nm + std::string("].quadratic")).c_str(), 0.0007f);
	}

	model = glm::mat4(1.0f);
	view = glm::mat4(1.0f);
	projection = glm::mat4(1.0f);

	model = glm::rotate(model, glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

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

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		if (cur_attitude <= 1.0f - (float)delta_time)
			cur_attitude += (float)delta_time;
		else
			cur_attitude = 1.0f;

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		if (cur_attitude >= (float)delta_time)
			cur_attitude -= (float)delta_time;
		else
			cur_attitude = 0.0f;

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

