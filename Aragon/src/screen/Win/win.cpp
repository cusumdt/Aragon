#include "screen\Win\win.h"

#include "raylib.h"

#include "Game\game.h"
#include "screen\Gameplay\gameplay.h"

namespace Game
{
	namespace initWin
	{
		static Texture2D fond;
		static Rectangle recExit;
		static Texture2D exit;
		static Texture2D negativeExit;
		static Texture2D menu;
		static Texture2D negativeMenu;
		static Rectangle recMenu;
		static bool menuButtonAnimationOn;
		static bool exitButtonAnimationOn;
		static bool firstInit = true;

		static void initWin()
		{
			if (firstInit)
			{
				firstInit = false;
				menu = LoadTexture("res/boton_menu.png");
				negativeMenu = LoadTexture("res/boton_menu2.png");
				exit = LoadTexture("res/return.png");
				negativeExit = LoadTexture("res/return2.png");
				fond = LoadTexture("res/menu.png");
				recMenu = { (float)halfScreenWidth - menu.width / 2,(float)screenHeight / 2 ,(float)menu.width,(float)menu.height };
				recExit = { (float)halfScreenWidth - exit.width / 2,(float)screenHeight / 2 + exit.height + 5,(float)exit.width,(float)exit.height };
			}
		}

		void UpdateWin()
		{
			initWin();
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
		void DrawWin()
		{
			static int fontSizeTitle = 50;
			static int fontSize = 20;
			DrawTexture(fond, 0, 0, WHITE);
			DrawText("You Win,Congrats!", halfScreenWidth  - (MeasureText("You Win,Congrats!", fontSizeTitle) / 2),screenHeight / 3, fontSizeTitle, GOLD);
			if (menuButtonAnimationOn)
			{
				DrawTexture(menu, halfScreenWidth  - menu.width / 2, screenHeight / 2, WHITE);
			}
			else
			{
				DrawTexture(negativeMenu, halfScreenWidth  - negativeMenu.width / 2, screenHeight / 2, WHITE);
			}
			if (exitButtonAnimationOn)
			{
				DrawTexture(exit, halfScreenWidth  - exit.width / 2, screenHeight / 2 + exit.height + 5, WHITE);
			}
			else
			{
				DrawTexture(negativeExit, halfScreenWidth  - exit.width / 2, screenHeight / 2 + exit.height + 5, WHITE);
			}
		}
		void closeWin() 
		{
			UnloadTexture(fond);
			UnloadTexture(menu);
			UnloadTexture(negativeMenu);
			UnloadTexture(exit);
			UnloadTexture(negativeExit);
		}
	}
}