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

#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

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
unsigned int VBO, VAO, lightVAO, lightVBO, lampVAO;
clock_t last_fps_tacts;
GLuint fps;
GLuint frames;
glm::vec3 lightPos(18.0f, 3.0f, 19.0f);

float cur_attitude = 0.2;

float delta_time = 0.0f;
float last_frame = 0.0f;
float current_frame = 0.0f;

bool init()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

	tsp::init("c:\\Users\\fghft\\source\\repos\\Opengl_project\\Fonts\\ariali.ttf", 48, SCR_WIDTH, SCR_HEIGHT);


	return true;
}

void draw()
{
	tex1->activateTexture();
	tex2->activateTexture();

	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

	view = myCamera.getViewMat();
	projection = glm::perspective(glm::radians(myCamera.getFov()), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);

	ourShader->use();
	ourShader->setUniformMat4("model", model);
	ourShader->setUniformMat4("view", view);
	ourShader->setUniformMat4("projection", projection);
	ourShader->setUniformValue("attitude", cur_attitude);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(2.0f, 1.0f, 1.0f));
	model = glm::rotate(model, glm::radians(100.0f), glm::vec3(0.7f, 1.0f, 0.3f));
	ourShader->setUniformMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	for (int k = 0; k < 5; ++k)
	{
		for (int j = 0; j < 50; ++j)
		{
			for (int i = 0; i < 50; ++i)
			{
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(1.0f * (float)j, -5.0f + ((float)-k * 1.0), 1.0f * (float)i));
				ourShader->setUniformMat4("model", model);
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		}
	}

	glm::vec3 final_lightPos = lightPos + glm::vec3(sin(glfwGetTime()) * 10.0f, 0.0f, cos(glfwGetTime()) * 10.0f);
	
	model = glm::mat4(1.0f);
	model = glm::translate(model, final_lightPos);
	model = glm::scale(model, glm::vec3(0.2f));

	lampShader->use();
	lampShader->setUniformMat4("model", model);
	lampShader->setUniformMat4("view", view);
	lampShader->setUniformMat4("projection", projection);
	glBindVertexArray(lampVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(16.0f, 1.0f, 20.0f));
	//model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));

	lightShader->use();
	lightShader->setUniformMat4("model", model);
	lightShader->setUniformMat4("view", view);
	lightShader->setUniformMat4("projection", projection);
	lightShader->setUniformVec3("light.position", glm::vec3(view * glm::vec4(final_lightPos, 1.0)));
	lightShader->setUniformVec3("viewPos", myCamera.getPos());
	container2_tex->activateTexture();
	container2_specular_tex->activateTexture();
	container2_emission_map->activateTexture();
	glBindVertexArray(lightVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
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

	std::vector<float> vertices;

	load_verticies("c:\\Users\\fghft\\source\\repos\\Opengl_project\\verticies\\verticies.txt", vertices);

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
	lightShader->setUniformVec3("lightColor", glm::vec3(0.5f));
	lightShader->setUniformVec3("material.specular", glm::vec3(0.50196078f));
	lightShader->setUniformValue("material.shininess", 32.0f);
	lightShader->setUniformVec3("light.ambient", glm::vec3(1.0f));
	lightShader->setUniformVec3("light.diffuse", glm::vec3(1.0f));
	lightShader->setUniformVec3("light.specular", glm::vec3(1.0f));

	const GLchar *t_path1 = "c:\\Users\\fghft\\source\\repos\\Opengl_project\\textures\\container.jpg";
	const GLchar *t_path2 = "c:\\Users\\fghft\\source\\repos\\Opengl_project\\textures\\awesomeface.png";
	const GLchar *t_path3 = "c:\\Users\\fghft\\source\\repos\\Opengl_project\\textures\\container_with_border.png";
	const GLchar *t_path4 = "c:\\Users\\fghft\\source\\repos\\Opengl_project\\textures\\container_with_border_specular.png";
	const GLchar *t_path5 = "c:\\Users\\fghft\\source\\repos\\Opengl_project\\textures\\matrix_emission_map.jpg";

	tex1 = new Texture(t_path1, "texture1");
	tex2 = new Texture(t_path2, "texture2", GL_RGBA, GL_TEXTURE1);
	container2_tex = new Texture(t_path3, "material.diffuse", GL_RGBA, GL_TEXTURE0);
	container2_specular_tex = new Texture(t_path4, "material.specular", GL_RGBA, GL_TEXTURE1);
	container2_emission_map = new Texture(t_path5, "material.emission", GL_RGB, GL_TEXTURE2);

	ourShader->use(); 
	glUniform1i(glGetUniformLocation(ourShader->ID, "texture1"), 0);
	glUniform1i(glGetUniformLocation(ourShader->ID, "texture2"), 1);

	lightShader->use();
	lightShader->setUniformValue("material.diffuse", 0);
	lightShader->setUniformValue("material.specular", 1);
	lightShader->setUniformValue("material.emission", 2);

	model = glm::mat4(1.0f);
	view = glm::mat4(1.0f);
	projection = glm::mat4(1.0f);

	model = glm::rotate(model, glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

	last_frame = glfwGetTime();
	last_fps_tacts = std::clock();

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		calc_fps();
		draw();
		draw_inf();

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

