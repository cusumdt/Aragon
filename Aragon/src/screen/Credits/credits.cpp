#include"credits.h"

#include "raylib.h"

#include "Game\game.h"
#include "screen\Gameplay\gameplay.h"

namespace Game
{
	namespace initCredits
	{
		static Texture2D fond;
		static Texture2D menu;
		static Texture2D negativeMenu;
		static Rectangle recMenu;
		static int positionY = screenHeight- screenHeight/100*60;
		static bool menuButtonAnimationOn;
		static bool exitButtonAnimationOn;
		static bool firstInit = true;
		static int fontSize = 20;

		static void initCredits()
		{
			if (firstInit)
			{
				firstInit = false;
				menu = LoadTexture("res/boton_menu.png");
				negativeMenu = LoadTexture("res/boton_menu2.png");
				fond = LoadTexture("res/bg/menu.png");
				recMenu = { (float)halfScreenWidth - menu.width / 2,(float)screenHeight- screenHeight / 4 ,(float)menu.width,(float)menu.height };
			}
		}
		void UpdateCredits()
		{
			initCredits();
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
		}
		void DrawCredits() 
		{
			DrawTexture(fond, 0, 0, WHITE);
			if (menuButtonAnimationOn)
			{
				DrawTexture(menu, halfScreenWidth - menu.width / 2, screenHeight - screenHeight / 4, WHITE);
			}
			else
			{
				DrawTexture(negativeMenu, halfScreenWidth  - negativeMenu.width / 2, screenHeight - screenHeight /4, WHITE);
			}
			DrawText("Programmer: Cusumano Cristian", halfScreenWidth - (MeasureText("Programmer: Cusumano Cristian", fontSize) / 2), positionY , fontSize, BEIGE);
			DrawText("Art, Music and Efects By: OpenGameArt And Cusumano Cristian", halfScreenWidth - (MeasureText("Music and Efects By: OpenGameArt And Cusumano Cristian", fontSize) / 2), positionY + fontSize*2 + 5, fontSize, BEIGE);
			DrawText("Tools: Raylib - Photoshop",halfScreenWidth - (MeasureText("Tools: Raylib - Photoshop", fontSize) / 2), positionY + fontSize * 4 + 5, fontSize, BEIGE);
			DrawText("v1.0", halfScreenWidth - (MeasureText("v1.0", fontSize) / 2), positionY + fontSize * 13 + 5, fontSize, BEIGE);
		}
	}
}