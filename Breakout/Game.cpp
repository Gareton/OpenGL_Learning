#include "Game.h"
#include "Resource_Manager.h"
#include "Sprite_Renderer.h"
#include <string>
#include <algorithm>

lamon::Game::Game(GLuint width, GLuint height)
{
	this->Width = width;
	this->Height = height;
}

lamon::Game::~Game()
{
	delete Renderer;
}

void lamon::Game::Init()
{
	ResourceManager::SetTexturesDir("c:\\Users\\fghft\\source\\repos\\Opengl_project\\Breakout_data\\Textures");
	ResourceManager::SetShadersDir("c:\\Users\\fghft\\source\\repos\\Opengl_project\\Breakout_data\\Shaders");
	ResourceManager::SetLevelsDir("c:\\Users\\fghft\\source\\repos\\Opengl_project\\Breakout_data\\Levels");

	ResourceManager::LoadShader("spriteShader.vert", "spriteShader.frag", nullptr, "spriteShader");

	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->Width), static_cast<GLfloat>(this->Height), 0.0f, -1.0f, 1.0f);

	ResourceManager::GetShader("spriteShader").Use().SetInteger("spriteTexture", 0);
	ResourceManager::GetShader("spriteShader").SetMatrix4("projection", projection);

	Renderer = new SpriteRenderer(ResourceManager::GetShader("spriteShader"));

	ResourceManager::LoadTexture("awesomeface.png", "awesomeFace");
	ResourceManager::LoadTexture("block.png", "block");
	ResourceManager::LoadTexture("block_solid.png", "solidBlock");
	ResourceManager::LoadTexture("background.jpg", "background");

	for (int i = 1; i <= 4; ++i)
	{
		GameLevel level;

		level.Load(("level" + std::to_string(i) + ".txt").c_str(), Width, Height * 0.5f);

		Levels.push_back(level);
	}

	Level = 0;
}

void lamon::Game::ProcessInput(GLfloat dt)
{

}

void lamon::Game::Update(GLfloat dt)
{

}

void lamon::Game::Render()
{
	if (this->State == GAME_ACTIVE)
	{
		Renderer->DrawSprite(ResourceManager::GetTexture("background"),
			glm::vec2(0, 0), glm::vec2(this->Width, this->Height), 0.0f
		);

		Levels[Level].Draw(*Renderer);
	}
}

void lamon::Game::SetState(lamon::GameState state)
{
	this->State = state;
}

void lamon::Game::SetKey(GLuint key, GLboolean state)
{
	this->Keys[key] = state;
}