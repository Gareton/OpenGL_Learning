#include "Game_Level.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "Resource_Manager.h"


void lamon::GameLevel::Load(const GLchar *file, GLuint levelWidth, GLuint levelHeight)
{
	Bricks.clear();

	GLuint tileCode;

	std::string line;
	std::ifstream istream(ResourceManager::GetLevelsDir() + "\\" + file);
	std::vector<std::vector<GLuint>> tileData;

	if (istream)
	{
		while (std::getline(istream, line)) 
		{
			std::istringstream sstream(line);
			std::vector<GLuint> row;
			while (sstream >> tileCode) 
				row.push_back(tileCode);
			tileData.push_back(row);
		}
		if (tileData.size() > 0)
			this->init(tileData, levelWidth, levelHeight);
	}
	else
	{
		std::cout << "ERROR::LOADING_FILE_FAILED::PATH: " << (ResourceManager::GetLevelsDir() + "\\" + file) << std::endl;
	}
}

void lamon::GameLevel::Draw(SpriteRenderer &renderer)
{
	for (GameObject &tile : this->Bricks)
		if (!tile.Destroyed)
			tile.Draw(renderer);
}

GLboolean lamon::GameLevel::IsCompleted()
{
	for (GameObject &tile : Bricks)
		if (!tile.IsSolid && !tile.Destroyed)
			return GL_FALSE;
	return GL_TRUE;
}

void lamon::GameLevel::init(std::vector<std::vector<GLuint>> tileData, GLuint levelWidth, GLuint levelHeight)
{
	GLuint height = tileData.size();
	GLuint width = tileData[0].size(); 
	GLfloat unit_width = levelWidth / static_cast<GLfloat>(width);
	GLfloat unit_height = levelHeight / static_cast<GLfloat>(height);

	for (GLuint y = 0; y < height; ++y)
	{
		for (GLuint x = 0; x < width; ++x)
		{

			if (tileData[y][x] == 1) 
			{
				glm::vec2 pos(unit_width * x, unit_height * y);
				glm::vec2 size(unit_width, unit_height);
				GameObject obj(pos, size, ResourceManager::GetTexture("solidBlock"), glm::vec3(0.8f, 0.8f, 0.7f));
				obj.IsSolid = GL_TRUE;
				this->Bricks.push_back(obj);
			}
			else if (tileData[y][x] > 1)	
			{
				glm::vec3 color = glm::vec3(1.0f); 
				if (tileData[y][x] == 2)
					color = glm::vec3(0.2f, 0.6f, 1.0f);
				else if (tileData[y][x] == 3)
					color = glm::vec3(0.0f, 0.7f, 0.0f);
				else if (tileData[y][x] == 4)
					color = glm::vec3(0.8f, 0.8f, 0.4f);
				else if (tileData[y][x] == 5)
					color = glm::vec3(1.0f, 0.5f, 0.0f);

				glm::vec2 pos(unit_width * x, unit_height * y);
				glm::vec2 size(unit_width, unit_height);
				this->Bricks.push_back(GameObject(pos, size, ResourceManager::GetTexture("block"), color));
			}
		}
	}
}

