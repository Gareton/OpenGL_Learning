#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H
#include <glad/glad.h>
#include <map>
#include "Texture.h"
#include "Shader.h"

class ResourceManager
{
public:
	static Shader      &LoadShader(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile, std::string name);

	static Shader      &GetShader(std::string name);

	static Texture2D   &LoadTexture(const GLchar *file, std::string name);

	static Texture2D   &GetTexture(std::string name);

	static void         SetTexturesDir(std::string dir);

	static void         SetShadersDir(std::string dir);

	static void		    SetLevelsDir(std::string dir);

	static void		    SetSoundsDir(std::string dir);

	static std::string	GetLevelsDir();

	static std::string	GetSoundsDir();

	static void      Clear();

	static std::string ShadersDir;
	static std::string TexturesDir;
	static std::string LevelsDir;
	static std::string SoundsDir;
private:
	ResourceManager() { }

	static Shader    loadShaderFromFile(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile = nullptr);
	static Texture2D loadTextureFromFile(const GLchar *file);
	static std::string loadFromFile(const std::string &path);

	static std::map<std::string, Shader>    Shaders;
	static std::map<std::string, Texture2D> Textures;
};

#endif