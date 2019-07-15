#ifndef GAME_H
#define GAME_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "Game_Level.h"

namespace lamon {

	enum GameState {
		GAME_ACTIVE,
		GAME_MENU,
		GAME_WIN
	};

	const glm::vec2 PLAYER_SIZE(100, 20);
	const GLfloat PLAYER_VELOCITY(500.0f);

	class Game
	{
	public:
		Game(GLuint width, GLuint height);
		~Game();

		void Init();

		void ProcessInput(GLfloat dt);
		void Update(GLfloat dt);
		void Render();

		void SetState(GameState state);
		void SetKey(GLuint key, GLboolean state);
	private:
		GameState              State;
		GLboolean              Keys[1024];
		GLuint                 Width, Height;
		std::vector<GameLevel> Levels;
		GLuint                 Level;
		SpriteRenderer  *Renderer;
	};

}


#endif