#include "Ball.h"
#include <glad/glad.h>

Ball::Ball()
	: GameObject(), Radius(12.5f), Stuck(true) { }

Ball::Ball(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture2D sprite)
	: GameObject(pos, glm::vec2(radius * 2, radius * 2), sprite, glm::vec3(1.0f), velocity), Radius(radius), Stuck(true), PassThrough(false), Sticky(false) { }

glm::vec2 Ball::Move(GLfloat dt, GLuint window_width)
{
	if (!Stuck)
	{
		Position += Velocity * dt;

		if (this->Position.x <= 0.0f)
		{
			this->Velocity.x = -this->Velocity.x;
			this->Position.x = 0.0f;
		}
		else if (this->Position.x + this->Size.x >= window_width)
		{
			this->Velocity.x = -this->Velocity.x;
			this->Position.x = window_width - this->Size.x;
		}
		if (this->Position.y <= 0.0f)
		{
			this->Velocity.y = -this->Velocity.y;
			this->Position.y = 0.0f;
		}
	}

	return Position;
}

void      Ball::Reset(glm::vec2 position, glm::vec2 velocity)
{
	this->Position = position;
	this->Velocity = velocity;
	this->Stuck = true;
}