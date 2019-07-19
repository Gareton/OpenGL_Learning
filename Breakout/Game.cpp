#include "Game.h"
#include "Resource_Manager.h"
#include "Sprite_Renderer.h"
#include "Particles.h"
#include <string>
#include <algorithm>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <irrKlang/irrKlang.h>
#include "TextRenderer.h"

lamon::Game::Game(GLuint width, GLuint height)
{
	this->Width = width;
	this->Height = height;
	ShakeTime = 0.05f;
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
	ResourceManager::SetSoundsDir("c:\\Users\\fghft\\source\\repos\\Opengl_project\\Breakout_data\\Sounds");

	ResourceManager::LoadShader("spriteShader.vert", "spriteShader.frag", nullptr, "spriteShader");
	ResourceManager::LoadShader("particleShader.vert", "particleShader.frag", nullptr, "particleShader");
	ResourceManager::LoadShader("postProcessing.vert", "postProcessing.frag", nullptr, "postProcessorShader");

	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->Width), static_cast<GLfloat>(this->Height), 0.0f, -1.0f, 1.0f);

	ResourceManager::GetShader("spriteShader").Use().SetInteger("spriteTexture", 0);
	ResourceManager::GetShader("spriteShader").SetMatrix4("projection", projection);
	ResourceManager::GetShader("particleShader").Use().SetInteger("sprite", 0);
	ResourceManager::GetShader("particleShader").SetMatrix4("projection", projection);

	Renderer = new SpriteRenderer(ResourceManager::GetShader("spriteShader"));

	ResourceManager::LoadTexture("awesomeface.png", "ball");
	ResourceManager::LoadTexture("block.png", "block");
	ResourceManager::LoadTexture("block_solid.png", "solidBlock");
	ResourceManager::LoadTexture("background.jpg", "background");
	ResourceManager::LoadTexture("paddle.png", "paddle");
	ResourceManager::LoadTexture("particle.png", "particle");
	ResourceManager::LoadTexture("powerup_chaos.png", "tex_chaos");
	ResourceManager::LoadTexture("powerup_confuse.png", "tex_confuse");
	ResourceManager::LoadTexture("powerup_increase.png", "tex_size");
	ResourceManager::LoadTexture("powerup_passthrough.png", "tex_pass");
	ResourceManager::LoadTexture("powerup_sticky.png", "tex_sticky");
	ResourceManager::LoadTexture("powerup_speed.png", "tex_speed");

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
	Particles = new ParticleGenerator(ResourceManager::GetShader("particleShader"), ResourceManager::GetTexture("particle"), 500);
	postProc = new PostProcessor(ResourceManager::GetShader("postProcessorShader"), Width, Height);

	SoundEngine = irrklang::createIrrKlangDevice();
	SoundEngine->play2D((ResourceManager::GetSoundsDir() + "\\" + "breakout_music.mp3").c_str(), GL_TRUE);

	textRenderer = new TextRenderer(Width, Height);
	textRenderer->Load("c:\\Users\\fghft\\source\\repos\\Opengl_project\\Breakout_data\\Fonts\\OCRAEXT.TTF", 24);

	State = GAME_MENU;
	Lives = 3;
	Level = 0;
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

	if (this->State == GAME_MENU)
	{
		if (this->Keys[GLFW_KEY_ENTER] && !KeysProcessed[GLFW_KEY_ENTER])
		{
			this->State = GAME_ACTIVE;
			KeysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
		}

		if (this->Keys[GLFW_KEY_W] && !KeysProcessed[GLFW_KEY_W])
		{
			this->Level = (this->Level + 1) % 4;
			KeysProcessed[GLFW_KEY_W] = GL_TRUE;
		}

		if (this->Keys[GLFW_KEY_S] && !KeysProcessed[GLFW_KEY_S])
		{
			if (this->Level > 0)
				--this->Level;
			else
				this->Level = 3;

			KeysProcessed[GLFW_KEY_S] = GL_TRUE;
		}
	}

	if (this->State == GAME_WIN)
	{
		if (this->Keys[GLFW_KEY_ENTER] && !KeysProcessed[GLFW_KEY_ENTER])
		{
			ResetLevel(Level);
			ResetPlayer();
			State = GAME_MENU;
			KeysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
		}
	}
}

void lamon::Game::Update(GLfloat dt)
{
	ball->Move(dt, Width);
	DoCollisions();
	Particles->Update(dt, *ball, 2, glm::vec2(ball->Radius / 2));

	if (ShakeTime > 0.0f)
	{
		ShakeTime -= dt;
		if (ShakeTime <= 0.0f)
			postProc->Shake = false;
	}

	UpdatePowerUps(dt);

	if (ball->Position.y >= Height + ball->Radius * 2.0f) 
	{
		--Lives;

		if (Lives == 0)
		{
			ResetLevel(Level);
			State = GAME_MENU;
		}
		ResetPlayer();
	}

	if (State == GAME_ACTIVE && Levels[Level].IsCompleted())
	{
		ResetLevel(Level);
		ResetPlayer();
		postProc->Chaos = GL_TRUE;
		State = GAME_WIN;
	}
}

void lamon::Game::Render()
{
	if (this->State == GAME_ACTIVE || this->State == GAME_MENU || this->State == GAME_WIN)
	{
		postProc->BeginRender();

		Renderer->DrawSprite(ResourceManager::GetTexture("background"),
			glm::vec2(0, 0), glm::vec2(this->Width, this->Height), 0.0f
		);

		Levels[Level].Draw(*Renderer);
		Player->Draw(*Renderer);

		for (power::PowerUp &powerUp : this->PowerUps)
			if (!powerUp.Destroyed)
				powerUp.Draw(*Renderer);

		Particles->Draw();
		ball->Draw(*Renderer);

		if (Levels[Level].IsCompleted())
			++Level;

		postProc->EndRender();

		postProc->Render(glfwGetTime());

		textRenderer->RenderText("Lives:" + std::to_string(Lives), 10.0f, 10.0f, 1, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	}

	if (this->State == GAME_MENU)
	{
		textRenderer->RenderText("Press ENTER to start", 250.0f, Height / 2, 1.0f);
		textRenderer->RenderText("Press W or S to select level", 245.0f, Height / 2 + 20.0f, 0.75f);
	}

	if (this->State == GAME_WIN)
	{
		textRenderer->RenderText(
			"You WON!!!", 320.0, Height / 2 - 20.0, 1.0, glm::vec4(0.0, 1.0, 0.0, 1.0) );

		textRenderer->RenderText(
			"Press ENTER to retry or ESC to quit", 130.0, Height / 2, 1.0, glm::vec4(1.0, 1.0, 0.0, 1.0) );
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

lamon::Collision lamon::Game::CheckBallCollision(Ball &one, GameObject &two)
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

void lamon::Game::ResetLevel(GLuint level)
{
	GameLevel temp;

	temp.Load(("level" + std::to_string(level + 1) + ".txt").c_str(), Width, Height * 0.5f);

	Levels[level] = temp;
	Level = level;
	Lives = 3;
}

void lamon::Game::ResetPlayer()
{
	glm::vec2 playerPos = glm::vec2(
		this->Width / 2 - PLAYER_SIZE.x / 2,
		this->Height - PLAYER_SIZE.y
	);

	Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);

	ball = new Ball(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("ball"));

	PowerUps.clear();

	postProc->Confuse = GL_FALSE;
	postProc->Chaos = GL_FALSE;
	postProc->Shake = GL_FALSE;
}

void lamon::Game::DoCollisions()
{
	GLboolean upCol = false;
	GLboolean downCol = false;
	GLboolean rightCol = false;
	GLboolean leftCol = false;

	Collision coll;

	for (GameObject &brick : Levels[Level].Bricks)
	{

		if (!brick.Destroyed)
		{
			coll = CheckBallCollision(*ball, brick);

			if (std::get<0>(coll))
			{
				if (!brick.IsSolid)
				{
					brick.Destroyed = true;
					SpawnPowerUps(brick);
					SoundEngine->play2D((ResourceManager::GetSoundsDir() + "\\" + "bleep.mp3").c_str(), GL_FALSE);
				}
				else
				{
					ShakeTime = 0.05f;
					postProc->Shake = GL_TRUE;
					SoundEngine->play2D((ResourceManager::GetSoundsDir() + "\\" + "solid.wav").c_str(), GL_FALSE);
				}


				Direction dir = std::get<1>(coll);

				glm::vec2 diff_vector = std::get<2>(coll);

				if (!ball->PassThrough || brick.IsSolid)
				{

					if (!((dir == LEFT && leftCol == GL_TRUE)
						|| (dir == RIGHT && rightCol == GL_TRUE)
						|| (dir == UP && upCol == GL_TRUE)
						|| (dir == DOWN && downCol == GL_TRUE)))
					{

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

					if (dir == UP)
					{
						upCol = true;
					}
					else if (dir == DOWN)
					{
						downCol = true;
					}
					else if (dir == RIGHT)
					{
						rightCol = true;
					}
					else
					{
						leftCol = true;
					}
				}
			}
		}

	}

	coll = CheckBallCollision(*ball, *Player);

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
		ball->Stuck = ball->Sticky;

		SoundEngine->play2D((ResourceManager::GetSoundsDir() + "\\" + "bleep.wav").c_str(), GL_FALSE);
	}

	for (power::PowerUp &powerUp : this->PowerUps)
	{
		if (!powerUp.Destroyed)
		{
			if (powerUp.Position.y >= this->Height)
				powerUp.Destroyed = GL_TRUE;
			if (CheckAABBCollision(*Player, powerUp))
			{	
				ActivatePowerUp(powerUp);
				powerUp.Destroyed = GL_TRUE;
				powerUp.Activated = GL_TRUE;
				SoundEngine->play2D((ResourceManager::GetSoundsDir() + "\\" + "powerup.wav").c_str(), GL_FALSE);
			}
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

GLboolean ShouldSpawn(GLuint chance)
{
	GLuint random = rand() % chance;
	return random == 0;
}

void lamon::Game::SpawnPowerUps(GameObject &block)
{
	if (ShouldSpawn(75)) 
		this->PowerUps.push_back(
			power::PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, block.Position, ResourceManager::GetTexture("tex_speed")
			));

	if (ShouldSpawn(75))
		this->PowerUps.push_back(
			power::PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f, block.Position, ResourceManager::GetTexture("tex_sticky")
			));

	if (ShouldSpawn(75))
		this->PowerUps.push_back(
			power::PowerUp("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f, block.Position, ResourceManager::GetTexture("tex_pass")
			));

	if (ShouldSpawn(75))
		this->PowerUps.push_back(
			power::PowerUp("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4), 0.0f, block.Position, ResourceManager::GetTexture("tex_size")
			));

	if (ShouldSpawn(15)) 
		this->PowerUps.push_back(
			power::PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 15.0f, block.Position, ResourceManager::GetTexture("tex_confuse")
			));

	if (ShouldSpawn(15))
		this->PowerUps.push_back(
			power::PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 15.0f, block.Position, ResourceManager::GetTexture("tex_chaos")
			));
}

void lamon::Game::UpdatePowerUps(GLfloat dt)
{
	for (power::PowerUp &powerUp : this->PowerUps)
	{
		powerUp.Position += powerUp.Velocity * dt;
		if (powerUp.Activated)
		{
			powerUp.Duration -= dt;

			if (powerUp.Duration <= 0.0f)
			{
				powerUp.Activated = GL_FALSE;

				if (powerUp.Type == "sticky")
				{
					if (!IsOtherPowerUpActive(this->PowerUps, "sticky"))
					{	
						ball->Sticky = GL_FALSE;
						Player->Color = glm::vec3(1.0f);
					}
				}
				else if (powerUp.Type == "pass-through")
				{
					if (!IsOtherPowerUpActive(this->PowerUps, "pass-through"))
					{	
						ball->PassThrough = GL_FALSE;
						ball->Color = glm::vec3(1.0f);
					}
				}
				else if (powerUp.Type == "confuse")
				{
					if (!IsOtherPowerUpActive(this->PowerUps, "confuse"))
					{	
						postProc->Confuse = GL_FALSE;
					}
				}
				else if (powerUp.Type == "chaos")
				{
					if (!IsOtherPowerUpActive(this->PowerUps, "chaos"))
					{	
						postProc->Chaos = GL_FALSE;
					}
				}
			}
		}
	}

	this->PowerUps.erase(std::remove_if(this->PowerUps.begin(), this->PowerUps.end(),
		[](const power::PowerUp &powerUp) { return powerUp.Destroyed && !powerUp.Activated; }
	), this->PowerUps.end());
}

bool lamon::Game::CheckAABBCollision(GameObject &one, GameObject &two)
{
	bool x_col = one.Position.x + one.Size.x >= two.Position.x && two.Position.x + two.Size.x >= one.Position.x;
	bool y_col = one.Position.y + one.Size.y >= two.Position.y && two.Position.y + two.Size.y >= one.Position.y;

	return x_col && y_col;
}

void lamon::Game::ActivatePowerUp(power::PowerUp &powerUp)
{
	if (powerUp.Type == "speed")
	{
		ball->Velocity *= 1.2;
	}
	else if (powerUp.Type == "sticky")
	{
		ball->Sticky = GL_TRUE;
		Player->Color = glm::vec3(1.0f, 0.5f, 1.0f);
	}
	else if (powerUp.Type == "pass-through")
	{
		ball->PassThrough = GL_TRUE;
		ball->Color = glm::vec3(1.0f, 0.5f, 0.5f);
	}
	else if (powerUp.Type == "pad-size-increase")
	{
		Player->Size.x += 50;
	}
	else if (powerUp.Type == "confuse")
	{
		if (!postProc->Chaos)
			postProc->Confuse = GL_TRUE;
	}
	else if (powerUp.Type == "chaos")
	{
		if (!postProc->Confuse)
			postProc->Chaos = GL_TRUE;
	}
}

GLboolean lamon::Game::IsOtherPowerUpActive(std::vector<power::PowerUp> &powerUps, std::string type)
{
	for (const power::PowerUp &powerUp : powerUps)
	{
		if (powerUp.Activated)
			if (powerUp.Type == type)
				return GL_TRUE;
	}

	return GL_FALSE;
}