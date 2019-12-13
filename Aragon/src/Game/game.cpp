#include "Game\game.h"

#include "raylib.h"

#include "screen\Menu\menu.h"
#include "screen\Win\win.h"
#include "screen\Gameplay\gameplay.h"
#include "screen\Credits\credits.h"
#include "screen\Defeat\defeat.h"

#define MUSIC_ON

namespace Game
{
	static void Init();
	static void Update();
	static void Draw();
	static void Close();
	bool endGame = false;
	Texture2D texture;
	Music MusicLoop;
	Music GameLoop;
	int screenWidth = 800;
	int screenHeight = 450;
	int halfScreenWidth = screenWidth / 2;
	bool Gameplay::firstInit = true;
	bool initMusic = true;
	Screen screen;
	void Init() 
	{
		InitWindow(screenWidth, screenHeight, "Dragons");
		screen = MENU;
#ifdef MUSIC_ON
		InitAudioDevice();
		MusicLoop = LoadMusicStream("res/music/intro.ogg");
		GameLoop = LoadMusicStream("res/music/gameplay.ogg");
		PlayMusicStream(MusicLoop);
#endif
	
	}

	void Update() 
	{
#ifdef MUSIC_ON
		if (music) 
		{
			
			UpdateMusicStream(screen == GAME ? GameLoop : MusicLoop);
		}
#endif
		switch (screen)
		{
		case GAME:
			if (Gameplay::firstInit)
			{
#ifdef MUSIC_ON
				ResumeMusicStream(MusicLoop);
				StopMusicStream(GameLoop);
				PlayMusicStream(GameLoop);
#endif
				Gameplay::initGame();
				Gameplay::firstInit = false;
				initMusic = true;
			}
			Gameplay::updateGame();
			break;
		case MENU:
			if (initMusic)
			{
#ifdef MUSIC_ON
				ResumeMusicStream(GameLoop);
				StopMusicStream(MusicLoop);
				PlayMusicStream(MusicLoop);
#endif
				initMusic = false;
			}
			initMenu::UpdateMenu();
			break;
		case WIN:
			initWin::UpdateWin();
			break;
		case CREDITS:
			initCredits::UpdateCredits();
			break;
		case DEFEAT:
			initDefeat::UpdateDefeat();
			break;
		default:
			break;
		}
	}
	
	void Draw() {
		BeginDrawing();
		ClearBackground(RAYWHITE);
		switch (screen)
		{
		case GAME:
			Gameplay::DrawGame();
			break;
		case MENU:
			initMenu::DrawMenu();
			break;
		case WIN:
			initWin::DrawWin();
			break;
		case CREDITS:
			initCredits::DrawCredits();
			break;
		case DEFEAT:
			initDefeat::DrawDefeat();
			break;
		}
		EndDrawing();
	}



	void Close() {
#ifdef MUSIC_ON
			UnloadMusicStream(MusicLoop);   // Unload music stream buffers from RAM
			CloseAudioDevice();
#endif
			UnloadTexture(texture);       // Texture unloading
			initMenu::UnloadTextureMenu();
			Gameplay::UnloadTexture();
			initWin::closeWin();
			initDefeat::CloseDefeat();
			CloseWindow();
	}
	void LoadScreen()
	{
		Init();
		SetTargetFPS(60);
		while (!WindowShouldClose() && !endGame)    
		{
			Update();
			Draw();
		}
		Close();
	}

}
