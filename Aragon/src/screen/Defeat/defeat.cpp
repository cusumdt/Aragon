#include "screen\Defeat\defeat.h"

#include "raylib.h"

#include "Game\game.h"
#include "screen\Gameplay\gameplay.h"

namespace Game
{
	namespace initDefeat
	{
		Texture2D fond;
		Rectangle recExit;
		Texture2D exit;
		Texture2D negativeExit;
		Texture2D menu;
		Texture2D negativeMenu;
		Rectangle recMenu;
		static bool menuButtonAnimationOn;
		static bool exitButtonAnimationOn;
		static bool firstInit = true;
		void initDefeat() 
		{
			if (firstInit)
			{
				firstInit = false;
				menu = LoadTexture("res/boton_menu.png");
				negativeMenu = LoadTexture("res/boton_menu2.png");
				exit = LoadTexture("res/return.png");
				negativeExit = LoadTexture("res/return2.png");
				fond = LoadTexture("res/bg/menu.png");
				recMenu = { (float)halfScreenWidth - menu.width / 2,(float)screenHeight / 2 ,(float)menu.width,(float)menu.height };
				recExit = { (float)halfScreenWidth - exit.width / 2,(float)screenHeight / 2 + exit.height + 5,(float)exit.width,(float)exit.height };
			}
		}
		void UpdateDefeat()
		{
			initDefeat();
			if (CheckCollisionPointRec(GetMousePosition(), recMenu))
			{
				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
				{
					screen = MENU;
				}
				menuButtonAnimationOn = false;
			}
			else
			{
				menuButtonAnimationOn = true;
			}
			if (CheckCollisionPointRec(GetMousePosition(), recExit))
			{
				exitButtonAnimationOn = false;

				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
				{
					screen = GAME;
				}
			}
			else
			{
				exitButtonAnimationOn = true;
			}
		}
		void DrawDefeat()
		{
			static int fontSizeTitle = 50;
			static int fontSize = 20;
			DrawTexture(fond, 0, 0, WHITE);
			DrawText("You Lose!", halfScreenWidth - (MeasureText("You Lose!", fontSizeTitle) / 2), screenHeight /3, fontSizeTitle, GOLD);
			if (menuButtonAnimationOn)
			{
				DrawTexture(menu, halfScreenWidth - menu.width / 2, screenHeight / 2, WHITE);
			}
			else
			{
				DrawTexture(negativeMenu, halfScreenWidth - negativeMenu.width / 2, screenHeight / 2, WHITE);
			}
			if (exitButtonAnimationOn)
			{
				DrawTexture(exit, halfScreenWidth - exit.width / 2,screenHeight / 2 + exit.height + 5, WHITE);
			}
			else
			{
				DrawTexture(negativeExit,halfScreenWidth  - exit.width / 2, screenHeight / 2 + exit.height + 5, WHITE);
			}
		}
		void CloseDefeat()
		{
			UnloadTexture(fond);
			UnloadTexture(menu);
			UnloadTexture(negativeMenu);
			UnloadTexture(exit);
			UnloadTexture(negativeExit);
		}
	}
}