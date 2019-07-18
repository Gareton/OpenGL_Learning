#include "Resource_Manager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "stb_image.h"

std::map<std::string, Texture2D>    ResourceManager::Textures;
std::map<std::string, Shader>       ResourceManager::Shaders;
std::string ResourceManager::ShadersDir;
std::string ResourceManager::TexturesDir;
std::string ResourceManager::LevelsDir;
std::string ResourceManager::SoundsDir;

Shader   &ResourceManager::LoadShader(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile, std::string name)
{
	if (Shaders.find(name) != Shaders.end())
	{
		std::cout << "Shader with name: " << name << " has already existed" << std::endl;
		return Shaders[name];
	}
	
	Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
	return Shaders[name];
}

Shader   &ResourceManager::GetShader(std::string name)
{
	if (Shaders.find(name) == Shaders.end())
	{
		std::cout << "Shader with name: " << name << " doesn't exist" << std::endl;
	}
	
	return Shaders[name];
}

Texture2D &ResourceManager::LoadTexture(const GLchar *file, std::string name)
{
	if (Textures.find(name) != Textures.end())
	{
		std::cout << "Textures with name: " << name << " has already existed" << std::endl;
		return Textures[name];
	}

	Textures[name] = loadTextureFromFile(file);
	return Textures[name];
}

Texture2D &ResourceManager::GetTexture(std::string name)
{
	if (Textures.find(name) == Textures.end())
	{
		std::cout << "Textures with name: " << name << " doesn't exist" << std::endl;
	}

	return Textures[name];
}

void      ResourceManager::Clear()
{
	if (Shaders.size() != 0)
	{
		for (auto it = ResourceManager::Shaders.begin(); it != ResourceManager::Shaders.end(); ++it)
		{
			glDeleteProgram(it->second.GetID());
		}
	}

	if (Textures.size() != 0)
	{
		for (auto el : Textures)
		{
			GLuint id = el.second.GetID();
			glDeleteTextures(1, &id);
		}
	}
}

Shader    ResourceManager::loadShaderFromFile(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile)
{
	std::string vShaderData = loadFromFile(ShadersDir + "\\" + vShaderFile);
	std::string fShaderData = loadFromFile(ShadersDir + "\\" + fShaderFile);
	std::string gShaderData;
	Shader sh;

	if (gShaderFile != nullptr)
	{
		gShaderData = loadFromFile(ShadersDir + "\\" + gShaderFile);

		sh.Compile(vShaderData.c_str(), fShaderData.c_str(), gShaderData.c_str());
	}
	else
	{
		sh.Compile(vShaderData.c_str(), fShaderData.c_str());
	}

	return sh;
}

Texture2D ResourceManager::loadTextureFromFile(const GLchar *file)
{
	int width, height, nrComponents;

	stbi_set_flip_vertically_on_load(false);

	unsigned char *data = stbi_load((TexturesDir + "\\" + file).c_str(), &width, &height, &nrComponents, 0);

	if (!data)
	{
		std::cout << "ERROR::FILE::FILE_NOT_SUCCESFULLY_READ" << std::endl << "PATH: " << (TexturesDir + "\\" + file) << std::endl;
	}

	Texture2D texture;

	texture.Generate(width, height, data, nrComponents);

	stbi_image_free(data);
	
	return texture;
}

std::string ResourceManager::loadFromFile(const std::string &path)
{
	std::ifstream in;
	std::stringstream fileStream;

	in.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		in.open(path);

		fileStream << in.rdbuf();

		in.close();
	}

	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::FILE::FILE_NOT_SUCCESFULLY_READ" << std::endl << "PATH: " << path << std::endl;
	}

	return fileStream.str();
}

void ResourceManager::SetTexturesDir(std::string dir)
{
	TexturesDir = dir;
}

void ResourceManager::SetShadersDir(std::string dir)
{
	ShadersDir = dir;
}

void ResourceManager::SetLevelsDir(std::string dir)
{
	LevelsDir = dir;
}

std::string	ResourceManager::GetLevelsDir()
{
	return LevelsDir;
}

void ResourceManager::SetSoundsDir(std::string dir)
{
	SoundsDir = dir;
}

std::string	ResourceManager::GetSoundsDir()
{
	return SoundsDir;
}
