#include "Game.h"
#include "Resource_Manager.h"
#include "Sprite_Renderer.h"
#include <string>
#include <algorithm>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

	ResourceManager::LoadTexture("awesomeface.png", "ball");
	ResourceManager::LoadTexture("block.png", "block");
	ResourceManager::LoadTexture("block_solid.png", "solidBlock");
	ResourceManager::LoadTexture("background.jpg", "background");
	ResourceManager::LoadTexture("paddle.png", "paddle");

	for (int i = 1; i <= 4; ++i)
	{
		GameLevel level;

		level.Load(("level" + std::to_string(i) + ".txt").c_str(), Width, Height * 0.5f);

		Levels.push_back(level);
	}

	glm::vec2 playerPos = glm::vec2(
		this->Width / 2 - PLAYER_SIZE.x / 2,
		this->Height - PLAYER_SIZE.y
	);

	Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);

	ball = new Ball(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY,  ResourceManager::GetTexture("ball"));

	Level = 2;
}

void lamon::Game::ProcessInput(GLfloat dt)
{
	if (this->State == GAME_ACTIVE)
	{
		GLfloat velocity = PLAYER_VELOCITY * dt;

		if (this->Keys[GLFW_KEY_A])
		{
			if (Player->Position.x >= 0)
			{
				Player->Position.x -= velocity;

				if (ball->Stuck)
					ball->Position.x -= velocity;
			}
		}
		if (this->Keys[GLFW_KEY_D])
		{
			if (Player->Position.x <= this->Width - Player->GetSize().x)
			{
				Player->Position.x += velocity;

				if (ball->Stuck)
					ball->Position.x += velocity;
			}
		}

		if (this->Keys[GLFW_KEY_SPACE])
			ball->Stuck = false;
	}
}

void lamon::Game::Update(GLfloat dt)
{
	ball->Move(dt, Width);
	DoCollisions();
}

void lamon::Game::Render()
{
	if (this->State == GAME_ACTIVE)
	{
		Renderer->DrawSprite(ResourceManager::GetTexture("background"),
			glm::vec2(0, 0), glm::vec2(this->Width, this->Height), 0.0f
		);

		Levels[Level].Draw(*Renderer);
		Player->Draw(*Renderer);
		ball->Draw(*Renderer);

		if (Levels[Level].IsCompleted())
			++Level;
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

lamon::Collision lamon::Game::CheckCollision(Ball &one, GameObject &two)
{
	glm::vec2 ballCenter = one.Position + glm::vec2(one.Radius);
	glm::vec2 brickCenter = two.Position + two.Size * glm::vec2(0.5f, 0.5f);
	glm::vec2 aabb_half_extents(two.Size.x * 0.5f, two.Size.y * 0.5f);
	glm::vec2 difference = ballCenter - brickCenter;
	glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
	glm::vec2 closest = brickCenter + clamped;

	difference = closest - ballCenter;

	if (glm::length(difference) <= one.Radius)
		return std::make_tuple(GL_TRUE, VectorDirection(difference), difference);
	else
		return std::make_tuple(GL_FALSE, UP, glm::vec2(0, 0));
}

void lamon::Game::DoCollisions()
{
	for (GameObject &brick : Levels[Level].Bricks)
	{
		Collision coll;

		if (!brick.Destroyed)
		{
			coll = CheckCollision(*ball, brick);

			if (std::get<0>(coll))
			{
				if (!brick.IsSolid)
					brick.Destroyed = true;

				Direction dir = std::get<1>(coll);

				glm::vec2 diff_vector = std::get<2>(coll);
				if (dir == LEFT || dir == RIGHT)
				{
					ball->Velocity.x = -ball->Velocity.x;

					GLfloat penetration = ball->Radius - std::abs(diff_vector.x);
					if (dir == LEFT)
						ball->Position.x += penetration;
					else
						ball->Position.x -= penetration;
				}
				else
				{
					ball->Velocity.y = -ball->Velocity.y;

					GLfloat penetration = ball->Radius - std::abs(diff_vector.y);
					if (dir == UP)
						ball->Position.y -= penetration;
					else
						ball->Position.y += penetration;
				}
			}
		}

		coll = CheckCollision(*ball, *Player);

		if (!ball->Stuck && std::get<0>(coll))
		{
			GLfloat centerBoard = Player->Position.x + Player->Size.x / 2;
			GLfloat distance = (ball->Position.x + ball->Radius) - centerBoard;
			GLfloat percentage = distance / (Player->Size.x / 2);

			GLfloat strength = 2.0f;
			glm::vec2 oldVelocity = ball->Velocity;
			ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
			ball->Velocity.y = -1 * abs(ball->Velocity.y); ;
			ball->Velocity = glm::normalize(ball->Velocity) * glm::length(oldVelocity);
		}
	}
}

lamon::Direction lamon::Game::VectorDirection(glm::vec2 target)
{
	static glm::vec2 compass[] = {
		glm::vec2(0.0f, 1.0f),	
		glm::vec2(1.0f, 0.0f),	
		glm::vec2(0.0f, -1.0f),	
		glm::vec2(-1.0f, 0.0f)	
	};
	GLfloat max = 0.0f;
	GLuint best_match = -1;
	for (GLuint i = 0; i < 4; i++)
	{
		GLfloat dot_product = glm::dot(glm::normalize(target), compass[i]);
		if (dot_product > max)
		{
			max = dot_product;
			best_match = i;
		}
	}
	return (Direction)best_match;
}