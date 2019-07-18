#ifndef BALL_H
#define BALL_H
#include "Game_Object.h"

class Ball : public GameObject
{
public:
	GLboolean Stuck;

	Ball();
	Ball(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture2D sprite);

	glm::vec2 Move(GLfloat dt, GLuint window_width);
	void      Reset(glm::vec2 position, glm::vec2 velocity);
	GLfloat   Radius;
	bool PassThrough;
	bool Sticky;
private:
};

#endif
