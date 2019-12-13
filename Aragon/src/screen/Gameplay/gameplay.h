#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include "raylib.h"

namespace Game
{
	namespace Gameplay
	{
		extern bool firstInit;
		extern bool left;
		extern bool right;
		void initGame();
		void updateGame();
		void DrawGame();
		struct Player {
			Rectangle rec;
			Vector2 speed;
			Color color;
		};
		extern Player player;
		extern float dragonHeight;
		void UnloadTexture();
		void RechargeMana();
		void Dash();
		void BackGround();
		void FirstWave();
		void SecondWave();
		void ThirdWave();
		void PlayerActions();
		void Behaviour();
		void ShootActions();
	}
}
#endif