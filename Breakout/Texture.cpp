#include "Texture.h"

Texture2D::Texture2D() : Width(0), Height(0), Internal_Format(GL_RGB), Image_Format(GL_RGB), Wrap_S(GL_REPEAT), Wrap_T(GL_REPEAT), Filter_Min(GL_LINEAR), Filter_Mag(GL_LINEAR)
{
	glGenTextures(1, &ID);
}

void Texture2D::Generate(GLuint width, GLuint height, unsigned char* data, GLuint channels)
{
	Width = width;
	Height = height;

	if (channels == 1)
	{
		Internal_Format = GL_RED;
		Image_Format = GL_RED;
	}
	else if (channels == 3)
	{
		Internal_Format = GL_RGB;
		Image_Format = GL_RGB;
	}
	else if (channels == 4)
	{
		Internal_Format = GL_RGBA;
		Image_Format = GL_RGBA;
	}

	glBindTexture(GL_TEXTURE_2D, ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, Wrap_S);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, Wrap_T);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Filter_Min);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Filter_Mag);

	glTexImage2D(GL_TEXTURE_2D, 0, Internal_Format, width, height, 0, Image_Format, GL_UNSIGNED_BYTE, data);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::Bind() const
{
	glBindTexture(GL_TEXTURE_2D, ID);
}

GLuint Texture2D::GetID()
{
	return ID;
}