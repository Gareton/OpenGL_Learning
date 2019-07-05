#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H
#include <glad/glad.h>
#include "stb_image.h"
#include <iostream>

class Texture
{
public:
	Texture(const GLchar *path = "c:\\Users\\fghft\\source\\repos\\Opengl_project\\textures\\container.jpg",
		const GLchar *name = "texture1",
		int color = GL_RGB,
		int texture_num = GL_TEXTURE0,
		int texture_type = GL_TEXTURE_2D);

	void setParameter(int option_type, int val);
	void activateTexture();
	void set(const GLchar *path = "c:\\Users\\fghft\\source\\repos\\Opengl_project\\textures\\container.jpg",
		const GLchar *name = "texture1",
		int color = GL_RGB,
		int texture_num = GL_TEXTURE0,
		int texture_type = GL_TEXTURE_2D);
private:
	unsigned int ID;
	int _t_type;
	int _t_num;

	void construct_helper(const GLchar *path, const GLchar *name, int color, int texture_num, int texture_type);
};

void Texture::set(const GLchar *path, const GLchar *name, int color, int texture_num, int texture_type)
{
	construct_helper(path, name, color, texture_num, texture_type);
}

void Texture::construct_helper(const GLchar *path, const GLchar *name, int color, int texture_num, int texture_type)
{
	_t_type = texture_type;
	_t_num = texture_num;

	glGenTextures(1, &ID);
	glBindTexture(texture_type, ID);

	glTexParameteri(texture_type, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(texture_type, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(texture_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(texture_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;

	stbi_set_flip_vertically_on_load(true);

	unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);

	if (data)
	{
		glTexImage2D(texture_type, 0, color, width, height, 0, color, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(texture_type);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);
}

Texture::Texture(const GLchar *path, const GLchar *name, int color, int texture_num, int texture_type)
{
	construct_helper(path, name, color, texture_num, texture_type);
}

void Texture::setParameter(int option_type, int val)
{
	glTexParameteri(_t_type, option_type, val);
}

void Texture::activateTexture()
{
	glActiveTexture(_t_num);
	glBindTexture(_t_type, ID);
}

#endif