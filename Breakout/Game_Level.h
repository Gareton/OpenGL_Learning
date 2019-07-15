#ifndef GAME_LEVEL_H
#define GAME_LEVEL_H
#include <vector>
#include "Sprite_Renderer.h"
#include "Game_Object.h"

namespace lamon {

	class GameLevel
	{
	public:
		GameLevel() { }

		void Load(const GLchar *file, GLuint levelWidth, GLuint levelHeight);

		void Draw(SpriteRenderer &renderer);

		GLboolean IsCompleted();

	private:
		void init(std::vector<std::vector<GLuint>> tileData, GLuint levelWidth, GLuint levelHeight);

		std::vector<GameObject> Bricks;
	};

}

#endif
