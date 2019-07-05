#ifndef SHADER_H
#define SHADER_H
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

const unsigned int infoSize = 512;

class Shader 
{
public:
	unsigned int ID;

	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	void use();
	void setUniformValue(const GLchar *name, int value) const;
	void setUniformValue(const GLchar *name, float value) const;
	void setUniformValue(const GLchar *name, bool value) const;
	void setUniformVec4(const GLchar *name, glm::vec4 v) const;
	void setUniformMat4(const GLchar *name, const glm::mat4 &m);
	void setUniformVec3(const GLchar *name, glm::vec3 v);
private:
	std::string loadFromFile(const std::string &path);
};

std::string Shader::loadFromFile(const std::string &path)
{
	std::ifstream in;
	std::stringstream shaderStream;

	in.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try 
	{
		in.open(path);

		shaderStream << in.rdbuf();
		
		in.close();
	}

	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	return shaderStream.str();
}

Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
{
	std::ifstream vertex_in(vertexPath);
	std::ifstream fragment_in(fragmentPath);
	std::string vertex_source = loadFromFile(vertexPath);
	std::string fragment_source = loadFromFile(fragmentPath);
	const char *cvertex_source = vertex_source.c_str();
	const char *cfragment_source = fragment_source.c_str();

	unsigned int vertex, fragment;
	char infoLog[infoSize];
	int success;

	memset(infoLog, static_cast<char>(0), infoSize);

	//vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &cvertex_source, NULL);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED" << std::endl;
		std::cout << "FILE PATH:" << std::string(vertexPath) << std::endl;
		std::cout << infoLog;
	};

	memset(infoLog, static_cast<char>(0), infoSize);
	
	//fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &cfragment_source, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED" << std::endl;
		std::cout << "FILE PATH:" << std::string(fragmentPath) << std::endl;
		std::cout << infoLog;
	};

	memset(infoLog, static_cast<char>(0), infoSize);

	//shader program

	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::use()
{
	glUseProgram(ID);
}

void Shader::setUniformValue(const GLchar *name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name), (int)value);
}
void Shader::setUniformValue(const GLchar *name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name), value);
}
void Shader::setUniformValue(const GLchar *name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name), value);
}

void Shader::setUniformVec4(const GLchar *name, glm::vec4 v) const
{
	glUniform4f(glGetUniformLocation(ID, name), v.x, v.y, v.z, v.w);
}

void Shader::setUniformMat4(const GLchar *name, const glm::mat4 &m)
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, glm::value_ptr(m));
}

void Shader::setUniformVec3(const GLchar *name, glm::vec3 v)
{
	glUniform3f(glGetUniformLocation(ID, name), v.x, v.y, v.z);
}

#endif
