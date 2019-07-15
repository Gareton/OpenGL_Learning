#include "Game.h"
#include "Resource_Manager.h"
#include "Sprite_Renderer.h"

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

	ResourceManager::LoadShader("spriteShader.vert", "spriteShader.frag", nullptr, "spriteShader");

	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->Width), static_cast<GLfloat>(this->Height), 0.0f, -1.0f, 1.0f);

	ResourceManager::GetShader("spriteShader").Use().SetInteger("spriteTexture", 0);
	ResourceManager::GetShader("spriteShader").SetMatrix4("projection", projection);

	Renderer = new SpriteRenderer(ResourceManager::GetShader("spriteShader"));

	ResourceManager::LoadTexture("awesomeface.png", "awesomeFace");
}

void lamon::Game::ProcessInput(GLfloat dt)
{

}

void lamon::Game::Update(GLfloat dt)
{

}

void lamon::Game::Render()
{
	Renderer->DrawSprite(ResourceManager::GetTexture("awesomeFace"), glm::vec2(200, 200), glm::vec2(300, 400), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
}

void lamon::Game::SetState(lamon::GameState state)
{
	this->State = state;
}

void lamon::Game::SetKey(GLuint key, GLboolean state)
{
	this->Keys[key] = state;
}