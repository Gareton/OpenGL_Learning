#ifndef GAME_H
#define GAME_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "Game_Level.h"
#include "Ball.h"
#include "Particles.h"

namespace lamon {

	enum GameState {
		GAME_ACTIVE,
		GAME_MENU,
		GAME_WIN
	};

	enum Direction {
		UP,
		RIGHT,
		DOWN,
		LEFT
	};

	typedef std::tuple<GLboolean, Direction, glm::vec2> Collision;

	const glm::vec2 PLAYER_SIZE(100, 20);
	const GLfloat BALL_RADIUS(20.0f);
	const GLfloat PLAYER_VELOCITY(500.0f);
	const glm::vec2 INITIAL_BALL_VELOCITY(110.0f, -350.0f);

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
		Collision CheckCollision(Ball &one, GameObject &two);
		void DoCollisions();
		Direction VectorDirection(glm::vec2 target);
	private:
		GameState              State;
		GLboolean              Keys[1024];
		GLuint                 Width, Height;
		std::vector<GameLevel> Levels;
		GLuint                 Level;
		SpriteRenderer  *Renderer;
		GameObject      *Player;
		Ball *ball;
		ParticleGenerator *Particles;
	};

}


#endif
