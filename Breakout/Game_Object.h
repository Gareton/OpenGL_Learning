#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Sprite_Renderer.h"
#include "Texture.h"

class GameObject
{
public:
	GameObject();
	GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));
	virtual void Draw(lamon::SpriteRenderer &renderer);
	glm::vec2 GetSize();

	GLboolean   IsSolid;
	GLboolean   Destroyed;
	glm::vec2   Position;
	glm::vec2   Size;
	glm::vec2   Velocity;
protected:
	glm::vec3   Color;

	GLfloat     Rotation;

	Texture2D   Sprite;
};

#endif
