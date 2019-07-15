#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Shader  &Shader::Use()
{
	glUseProgram(ID);
	return *this;
}

void    Shader::Compile(const GLchar *vertexSource, const GLchar *fragmentSource, const GLchar *geometrySource)
{
	GLuint vShader, fShader, gShader;

	vShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vShader, 1, &vertexSource, nullptr);
	glCompileShader(vShader);
	checkCompileErrors(vShader, "VERTEX");

	fShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fShader, 1, &fragmentSource, nullptr);
	glCompileShader(fShader);
	checkCompileErrors(fShader, "FRAGMENT");

	if (geometrySource != nullptr)
	{
		gShader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(gShader, 1, &geometrySource, nullptr);
		glCompileShader(gShader);
		checkCompileErrors(gShader, "GEOMETRY");
	}

	ID = glCreateProgram();
	glAttachShader(ID, vShader);
	glAttachShader(ID, fShader);

	if(geometrySource != nullptr)
		glAttachShader(ID, gShader);

	glLinkProgram(ID);
	checkCompileErrors(ID, "PROGRAM");

	glDeleteShader(vShader);
	glDeleteShader(fShader);

	if (geometrySource != nullptr)
		glDeleteShader(gShader);
}

void    Shader::SetFloat(const GLchar *name, GLfloat value, GLboolean useShader)
{
	if (useShader)
		this->Use();
	glUniform1f(glGetUniformLocation(ID, name), value);
}

void    Shader::SetInteger(const GLchar *name, GLint value, GLboolean useShader)
{
	if (useShader)
		this->Use();
	glUniform1i(glGetUniformLocation(ID, name), value);
}

void    Shader::SetVector2f(const GLchar *name, GLfloat x, GLfloat y, GLboolean useShader)
{
	if (useShader)
		this->Use();
	glUniform2f(glGetUniformLocation(ID, name), x, y);
}

void    Shader::SetVector2f(const GLchar *name, const glm::vec2 &value, GLboolean useShader)
{
	if (useShader)
		this->Use();
	glUniform2f(glGetUniformLocation(ID, name), value.x, value.y);
}

void    Shader::SetVector3f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader)
{
	if (useShader)
		this->Use();
	glUniform3f(glGetUniformLocation(ID, name), x, y, z);
}

void    Shader::SetVector3f(const GLchar *name, const glm::vec3 &value, GLboolean useShader)
{
	if (useShader)
		this->Use();
	glUniform3f(glGetUniformLocation(ID, name), value.x, value.y, value.z);
}

void    Shader::SetVector4f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean useShader)
{
	if (useShader)
		this->Use();
	glUniform4f(glGetUniformLocation(ID, name), x, y, z, w);
}

void    Shader::SetVector4f(const GLchar *name, const glm::vec4 &value, GLboolean useShader)
{
	if (useShader)
		this->Use();
	glUniform4f(glGetUniformLocation(ID, name), value.x, value.y, value.z, value.w);
}

void    Shader::SetMatrix4(const GLchar *name, const glm::mat4 &matrix, GLboolean useShader)
{
	if (useShader)
		this->Use();
	glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, glm::value_ptr(matrix));
}

GLuint Shader::GetID() const
{
	return ID;
}

void    Shader::checkCompileErrors(GLuint object, std::string type)
{

	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(object, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(object, 1024, NULL, infoLog);
			std::cout << "| ERROR::SHADER: Compile-time error: Type: " << type << "\n"
				<< infoLog << "\n -- --------------------------------------------------- -- "
				<< std::endl;
		}
	}
	else
	{
		glGetProgramiv(object, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(object, 1024, NULL, infoLog);
			std::cout << "| ERROR::Shader: Link-time error: Type: " << type << "\n"
				<< infoLog << "\n -- --------------------------------------------------- -- "
				<< std::endl;
		}
	}
}