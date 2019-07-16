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

		std::vector<GameObject> Bricks;
	private:
		void init(std::vector<std::vector<GLuint>> tileData, GLuint levelWidth, GLuint levelHeight);
	};

}

#endif
