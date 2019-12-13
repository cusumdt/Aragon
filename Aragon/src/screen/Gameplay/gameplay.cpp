#include "gameplay.h"
#include "Game\game.h"
#include <iostream>
namespace Game
{
	namespace Gameplay
	{

#define NUM_SHOOTS 50
#define NUM_MAX_ENEMIES 50
#define FIRST_WAVE 30
#define SECOND_WAVE 40
#define THIRD_WAVE 50
#define MAX_PARTICLES 200
#define DASH_COST 20
#define LIMIT_MANA 100
#define NUM_PU 2
		// Particle structure with basic data
		typedef struct Particle {
			Vector2 position;
			Color color;
			float alpha;
			float size;
			float rotation;
			bool active;        // NOTE: Use it to activate/deactive particle
		} Particle;
		typedef enum { FIRST = 0, SECOND, THIRD } EnemyWave;


		typedef struct Enemy {
			Rectangle rec;
			Vector2 speed;
			bool active;
			Color color;
			Rectangle sourceRec;
			Rectangle destRec;
			Vector2 origin;
		} Enemy;

		typedef struct PowerUp {
			Rectangle rec;
			Vector2 speed;
			bool active;
			Color color;
			Rectangle sourceRec;
			Rectangle destRec;
			Vector2 origin;
		} PowerUp;

		typedef struct Shoot {
			Rectangle rec;
			Vector2 speed;
			bool active;
			Color color;
		} Shoot;

		//------------------------------------------------------------------------------------
		// Global Variables Declaration
		//------------------------------------------------------------------------------------

		int screenWidth = 800;
		int screenHeight = 450;

		static bool gameOver = false;
		static bool pause = false;
		static int score = 0;
		static bool victory = false;

		static Enemy enemy[NUM_MAX_ENEMIES];
		static Shoot shoot[NUM_SHOOTS];
		static Shoot shootDiagonalTop[NUM_SHOOTS];
		static Shoot shootDiagonalDown[NUM_SHOOTS];
		static Shoot shootDiagonalMidTop[NUM_SHOOTS];
		static Shoot shootDiagonalMidDown[NUM_SHOOTS];
		static PowerUp powerUp[NUM_PU];
		static EnemyWave wave;

		static int shootRate = 0;
		static float alpha = 0.0f;

		static int activeEnemies = 0;
		static int activePU = 0;
		static int enemiesKill = 0;
		static bool smooth = false;

		Player player;
		Texture2D ship;
		Texture2D shipMovement;
		Texture2D shipShoot;
		Texture2D background;
		Texture2D midground;
		Texture2D foreground;
		Texture2D meteor;
		Texture2D po;


		int currentFrame = 0;
		int framesCounter = 0;
		int framesSpeed = 8;
		float scrollingBack = 0.0f;
		float scrollingMid = 0.0f;
		float scrollingFore = 0.0f;
		bool dash = false;
		float time = 0;
		Particle mouseTail[MAX_PARTICLES] = { 0 };
		float gravity;
		Texture2D smoke;
		int blending;
		Vector2 positionExplo;
		bool activeExplo = false;
		float mana;
		int powerUpTier;
		void initGame()
		{
			mana = LIMIT_MANA;
			// Initialize particles
			for (int i = 0; i < MAX_PARTICLES; i++)
			{
				mouseTail[i].position = Vector2({ 0, 0 });
				mouseTail[i].color = WHITE;
				mouseTail[i].alpha = 1.0f;
				mouseTail[i].size = (float)GetRandomValue(1, 30) / 20.0f;
				mouseTail[i].rotation = 0.0f;
				mouseTail[i].active = false;
			}

			gravity = 3.0f;
			blending = BLEND_ALPHA;
			//------------------------
			// Initialize game variables
			shootRate = 0;
			pause = false;
			gameOver = false;
			victory = false;
			smooth = false;
			wave = FIRST;
			activeEnemies = FIRST_WAVE;
			enemiesKill = 0;
			score = 0;
			alpha = 0;
			//------------------------
			// Initialize player
			player.rec.x = 20;
			player.rec.y = 50;
			player.rec.width = 80;
			player.rec.height = 40;
			player.speed.x = 5;
			player.speed.y = 5;
			player.color = BLACK;
			//------------------------
			//Init Textures 
			smoke = LoadTexture("res/flash/m_2.png");
			po = LoadTexture("res/powerUp/po.png");
			ship = LoadTexture("res/blueship/blueship1.png");
			shipMovement = LoadTexture("res/blueship/blueship4.png");
			shipShoot = LoadTexture("res/blueship/blueship3.png");
			background = LoadTexture("res/bg/bg.png");
			midground = LoadTexture("res/bg/mid.png");
			foreground = LoadTexture("res/bg/front.png");
			meteor = LoadTexture("res/Meteor/met1.png");
			//------------------------
			// Initialize enemies
			for (int i = 0; i < NUM_MAX_ENEMIES; i++)
			{
				enemy[i].rec.width = 40;
				enemy[i].rec.height = 40;
				enemy[i].rec.x = GetRandomValue(screenWidth, screenWidth + 1000);
				enemy[i].rec.y = GetRandomValue(0, screenHeight - enemy[i].rec.height);
				enemy[i].speed.x = 5;
				enemy[i].speed.y = 5;
				enemy[i].active = true;
				enemy[i].color = GRAY;
				enemy[i].sourceRec = { 0.0f,0.0f,(float)meteor.width,(float)meteor.height };
				enemy[i].destRec = { enemy[i].rec.x,enemy[i].rec.y,(float)meteor.width,(float)meteor.height };
				enemy[i].origin = { (float)meteor.width / 2,(float)meteor.height / 2 };
			}
			//------------------------
			// Initialize Power UPS
			for (int i = 0; i < NUM_PU; i++)
			{
				powerUp[i].rec.width = 40;
				powerUp[i].rec.height = 40;
				powerUp[i].rec.x = GetRandomValue(screenWidth, screenWidth + 1000);
				powerUp[i].rec.y = GetRandomValue(0, screenHeight - powerUp[i].rec.height);
				powerUp[i].speed.x = 5;
				powerUp[i].speed.y = 5;
				powerUp[i].active = true;
				powerUp[i].color = GRAY;
				powerUp[i].sourceRec = { 0.0f,0.0f,(float)meteor.width,(float)meteor.height };
				powerUp[i].destRec = { enemy[i].rec.x,enemy[i].rec.y,(float)meteor.width,(float)meteor.height };
				powerUp[i].origin = { (float)meteor.width / 2,(float)meteor.height / 2 };
			}
			// Initialize shoots
			for (int i = 0; i < NUM_SHOOTS; i++)
			{
				shoot[i].rec.x = player.rec.x;
				shoot[i].rec.y = player.rec.y + player.rec.height / 4;
				shoot[i].rec.width = 10;
				shoot[i].rec.height = 5;
				shoot[i].speed.x = 7;
				shoot[i].speed.y = 0;
				shoot[i].active = false;
				shoot[i].color = RAYWHITE;
				shootDiagonalTop[i].rec.x = player.rec.x;
				shootDiagonalTop[i].rec.y = player.rec.y + player.rec.height / 4;
				shootDiagonalTop[i].rec.width = 10;
				shootDiagonalTop[i].rec.height = 5;
				shootDiagonalTop[i].speed.x = 7;
				shootDiagonalTop[i].speed.y = 7;
				shootDiagonalTop[i].active = false;
				shootDiagonalTop[i].color = RAYWHITE;
				shootDiagonalDown[i].rec.x = player.rec.x;
				shootDiagonalDown[i].rec.y = player.rec.y + player.rec.height / 4;
				shootDiagonalDown[i].rec.width = 10;
				shootDiagonalDown[i].rec.height = 5;
				shootDiagonalDown[i].speed.x = 7;
				shootDiagonalDown[i].speed.y = 7;
				shootDiagonalDown[i].active = false;
				shootDiagonalDown[i].color = RAYWHITE;
				shootDiagonalMidTop[i].rec.x = player.rec.x;
				shootDiagonalMidTop[i].rec.y = player.rec.y + player.rec.height / 4;
				shootDiagonalMidTop[i].rec.width = 10;
				shootDiagonalMidTop[i].rec.height = 5;
				shootDiagonalMidTop[i].speed.x = 7;
				shootDiagonalMidTop[i].speed.y = 7;
				shootDiagonalMidTop[i].active = false;
				shootDiagonalMidTop[i].color = RAYWHITE;
				shootDiagonalMidDown[i].rec.x = player.rec.x;
				shootDiagonalMidDown[i].rec.y = player.rec.y + player.rec.height / 4;
				shootDiagonalMidDown[i].rec.width = 10;
				shootDiagonalMidDown[i].rec.height = 5;
				shootDiagonalMidDown[i].speed.x = 7;
				shootDiagonalMidDown[i].speed.y = 7;
				shootDiagonalMidDown[i].active = false;
				shootDiagonalMidDown[i].color = RAYWHITE;
			}
		}

		void updateGame()
		{
			if (!gameOver)
			{

				if (IsKeyPressed('P'))
					pause = !pause;
				if (!pause)
				{
					RechargeMana();
					Dash();
					BackGround();

					switch (wave)
					{
					case FIRST:
					{
						FirstWave();
					} break;
					case SECOND:
					{
						SecondWave();
					} break;
					case THIRD:
					{
						ThirdWave();
					} break;
					default: break;
					}

					PlayerActions();

					Behaviour();
					ShootActions();
				}
			}
			else
				if (IsKeyPressed(KEY_ENTER))
				{
					gameOver = false;
					firstInit = true;
					activePU = 0;
					powerUpTier = 0;
					screen = DEFEAT;
				}
		}

		void RechargeMana()
		{
			if (mana > LIMIT_MANA)
				mana = LIMIT_MANA;
			else
				mana += 0.05f;
		}

		void Dash()
		{
			if (dash)
			{
				time += 0.1f;
				if (time < 1)
				{
					player.rec.x += player.speed.x * 5;
				}
				else
				{
					dash = false;
					time = 0;
				}
				for (int i = 0; i < MAX_PARTICLES; i++)
				{
					if (!mouseTail[i].active)
					{
						mouseTail[i].active = true;
						mouseTail[i].alpha = 1.0f;
						mouseTail[i].position = Vector2({ player.rec.x, player.rec.y + player.rec.height / 2 });
						i = MAX_PARTICLES;
					}
				}

				for (int i = 0; i < MAX_PARTICLES; i++)
				{
					if (mouseTail[i].active)
					{
						mouseTail[i].position.y = player.rec.y + player.rec.height / 2;
						mouseTail[i].position.x -= gravity;

						mouseTail[i].alpha -= 0.01f;

						if (mouseTail[i].alpha <= 0.0f) mouseTail[i].active = false;

					}
				}
			}
			else
			{
				for (int i = 0; i < MAX_PARTICLES; i++)
				{
					if (mouseTail[i].active)
					{
						mouseTail[i].active = false;
					}
				}
			}
		}

		void BackGround()
		{
			scrollingBack -= 0.1f;
			scrollingMid -= 0.5f;
			scrollingFore -= 1.0f;
			if (scrollingBack <= -background.width * 2) scrollingBack = 0;
			if (scrollingMid <= -midground.width * 2) scrollingMid = 0;
			if (scrollingFore <= -foreground.width * 2) scrollingFore = 0;

		}

		void FirstWave()
		{
			if (!smooth)
			{
				alpha += 0.02f;

				if (alpha >= 1.0f) smooth = true;
			}

			if (smooth) alpha -= 0.02f;

			if (enemiesKill == activeEnemies)
			{
				enemiesKill = 0;

				for (int i = 0; i < activeEnemies; i++)
				{
					if (!enemy[i].active) enemy[i].active = true;
				}

				activeEnemies = SECOND_WAVE;
				activePU = 0;
				wave = SECOND;
				smooth = false;
				alpha = 0.0f;
			}
		}

		void SecondWave()
		{
			if (!smooth)
			{
				alpha += 0.02f;

				if (alpha >= 1.0f) smooth = true;
			}

			if (smooth) alpha -= 0.02f;
			activePU = 1;
			if (enemiesKill == activeEnemies)
			{
				enemiesKill = 0;

				for (int i = 0; i < activeEnemies; i++)
				{
					if (!enemy[i].active) enemy[i].active = true;
				}

				activeEnemies = THIRD_WAVE;

				wave = THIRD;
				smooth = false;
				alpha = 0.0f;
			}
		}

		void ThirdWave()
		{
			if (!smooth)
			{
				alpha += 0.02f;

				if (alpha >= 1.0f) smooth = true;
			}

			if (smooth) alpha -= 0.02f;
			activePU = 1;
			if (enemiesKill == activeEnemies)
			{
				victory = true;
				score += 10000;
			}

		}

		void PlayerActions()
		{
			// Player movement
			if (IsKeyDown(KEY_RIGHT)) player.rec.x += player.speed.x;
			if (IsKeyDown(KEY_LEFT)) player.rec.x -= player.speed.x;
			if (IsKeyDown(KEY_UP)) player.rec.y -= player.speed.y;
			if (IsKeyDown(KEY_DOWN)) player.rec.y += player.speed.y;
			if (IsKeyPressed(KEY_LEFT_SHIFT))
			{
				if (mana >= DASH_COST)
				{
					dash = true;
					mana -= DASH_COST;
				}
			}

			// Player collision with enemy
			for (int i = 0; i < activeEnemies; i++)
			{
				if (!dash)
				{
					if (CheckCollisionRecs(player.rec, enemy[i].rec)) gameOver = true;
				}
				else
				{
					if (CheckCollisionRecs(player.rec, enemy[i].rec))
					{

						if (enemy[i].active)
						{
							enemy[i].rec.x = GetRandomValue(screenWidth, screenWidth + 1000);
							enemy[i].rec.y = GetRandomValue(0, screenHeight - enemy[i].rec.height);
							enemiesKill++;
							score += 100;
						}

					}
				}

			}
			for (int i = 0; i < activePU; i++)
			{
				if (!dash)
				{
					if (CheckCollisionRecs(player.rec, powerUp[i].rec))
					{
						score += 1000;
						powerUp[i].rec.x = GetRandomValue(screenWidth, screenWidth + 1000);
						powerUp[i].rec.y = GetRandomValue(0, screenHeight - powerUp[i].rec.height);
						if (powerUpTier < 2)
							powerUpTier++;
					}
				}

			}
		}

		void Behaviour()
		{
			// Enemy behaviour
			for (int i = 0; i < activeEnemies; i++)
			{
				if (enemy[i].active)
				{
					enemy[i].rec.x -= enemy[i].speed.x;

					if (enemy[i].rec.x < 0)
					{
						enemy[i].rec.x = GetRandomValue(screenWidth, screenWidth + 1000);
						enemy[i].rec.y = GetRandomValue(0, screenHeight - enemy[i].rec.height);
					}
					enemy[i].destRec = { enemy[i].rec.x,enemy[i].rec.y,(float)meteor.width,(float)meteor.height };
				}
			}

			for (int i = 0; i < activePU; i++)
			{
				if (powerUp[i].active)
				{
					powerUp[i].rec.x -= powerUp[i].speed.x;

					if (powerUp[i].rec.x < 0)
					{
						powerUp[i].rec.x = GetRandomValue(screenWidth, screenWidth + 1000);
						powerUp[i].rec.y = GetRandomValue(0, screenHeight - powerUp[i].rec.height);
					}
					powerUp[i].destRec = { powerUp[i].rec.x,powerUp[i].rec.y,(float)po.width,(float)po.height };
				}
			}
			// Wall behaviour
			if (player.rec.x <= 0) player.rec.x = 0;
			if (player.rec.x + player.rec.width >= screenWidth) player.rec.x = screenWidth - player.rec.width;
			if (player.rec.y <= 0) player.rec.y = 0;
			if (player.rec.y + player.rec.height >= screenHeight) player.rec.y = screenHeight - player.rec.height;

		}

		void ShootActions()
		{
			// Shoot initialization
			if (IsKeyDown(KEY_SPACE))
			{
				shootRate += 5;

				for (int i = 0; i < NUM_SHOOTS; i++)
				{
					if (!shoot[i].active && !shootDiagonalTop[i].active && !shootDiagonalDown[i].active && !shootDiagonalMidDown[i].active && !shootDiagonalMidTop[i].active && !shootDiagonalMidTop[i].active && shootRate % 20 == 0)
					{
						shoot[i].rec.x = player.rec.x + player.rec.width - 20;
						shoot[i].rec.y = player.rec.y + player.rec.height / 2;
						shoot[i].active = true;
						if (powerUpTier >= 1)
						{
							shootDiagonalTop[i].rec.x = player.rec.x + player.rec.width - 20;
							shootDiagonalTop[i].rec.y = player.rec.y + player.rec.height / 2;
							shootDiagonalTop[i].active = true;
							shootDiagonalDown[i].rec.x = player.rec.x + player.rec.width - 20;
							shootDiagonalDown[i].rec.y = player.rec.y + player.rec.height / 2;
							shootDiagonalDown[i].active = true;
						}
						if (powerUpTier == 2)
						{
							shootDiagonalMidDown[i].rec.x = player.rec.x + player.rec.width - 20;
							shootDiagonalMidDown[i].rec.y = player.rec.y + player.rec.height / 2;
							shootDiagonalMidDown[i].active = true;
							shootDiagonalMidTop[i].rec.x = player.rec.x + player.rec.width - 20;
							shootDiagonalMidTop[i].rec.y = player.rec.y + player.rec.height / 2;
							shootDiagonalMidTop[i].active = true;
						}
						break;
					}




				}
			}

			// Shoot logic
			for (int i = 0; i < NUM_SHOOTS; i++)
			{
				if (shoot[i].active)
				{
					// Movement
					shoot[i].rec.x += shoot[i].speed.x;

					// Collision with enemy
					for (int j = 0; j < activeEnemies; j++)
					{
						if (enemy[j].active)
						{
							if (CheckCollisionRecs(shoot[i].rec, enemy[j].rec))
							{
								shoot[i].active = false;
								enemy[j].rec.x = GetRandomValue(screenWidth, screenWidth + 1000);
								enemy[j].rec.y = GetRandomValue(0, screenHeight - enemy[j].rec.height);
								shootRate = 0;
								enemiesKill++;
								score += 100;
							}

							if (shoot[i].rec.x + shoot[i].rec.width >= screenWidth)
							{
								shoot[i].active = false;
								shootRate = 0;
							}
						}
					}
				}
			}
			//Shoot diagonalTop
			for (int i = 0; i < NUM_SHOOTS; i++)
			{
				if (shootDiagonalTop[i].active)
				{
					// Movement
					shootDiagonalTop[i].rec.x += shootDiagonalTop[i].speed.x;
					shootDiagonalTop[i].rec.y -= shootDiagonalTop[i].speed.y / 2;

					// Collision with enemy
					for (int j = 0; j < activeEnemies; j++)
					{
						if (enemy[j].active)
						{
							if (CheckCollisionRecs(shootDiagonalTop[i].rec, enemy[j].rec))
							{
								shootDiagonalTop[i].active = false;
								enemy[j].rec.x = GetRandomValue(screenWidth, screenWidth + 1000);
								enemy[j].rec.y = GetRandomValue(0, screenHeight - enemy[j].rec.height);
								shootRate = 0;
								enemiesKill++;
								score += 100;
							}

							if (shootDiagonalTop[i].rec.x + shootDiagonalTop[i].rec.width >= screenWidth)
							{
								shootDiagonalTop[i].active = false;
								shootRate = 0;
							}
						}
					}
				}
			}
			//Shoot diagonalDown
			for (int i = 0; i < NUM_SHOOTS; i++)
			{
				if (shootDiagonalDown[i].active)
				{
					// Movement
					shootDiagonalDown[i].rec.x += shootDiagonalDown[i].speed.x;
					shootDiagonalDown[i].rec.y += shootDiagonalDown[i].speed.y / 2;

					// Collision with enemy
					for (int j = 0; j < activeEnemies; j++)
					{
						if (enemy[j].active)
						{
							if (CheckCollisionRecs(shootDiagonalDown[i].rec, enemy[j].rec))
							{
								shootDiagonalDown[i].active = false;
								enemy[j].rec.x = GetRandomValue(screenWidth, screenWidth + 1000);
								enemy[j].rec.y = GetRandomValue(0, screenHeight - enemy[j].rec.height);
								shootRate = 0;
								enemiesKill++;
								score += 100;
							}

							if (shootDiagonalDown[i].rec.x + shootDiagonalDown[i].rec.width >= screenWidth)
							{
								shootDiagonalDown[i].active = false;
								shootRate = 0;
							}
						}
					}
				}
				//mid down

			}
			//mid down
			for (int i = 0; i < NUM_SHOOTS; i++)
			{
				if (shootDiagonalMidDown[i].active)
				{
					// Movement
					shootDiagonalMidDown[i].rec.x += shootDiagonalMidDown[i].speed.x;
					shootDiagonalMidDown[i].rec.y += shootDiagonalMidDown[i].speed.y / 4;

					// Collision with enemy
					for (int j = 0; j < activeEnemies; j++)
					{
						if (enemy[j].active)
						{
							if (CheckCollisionRecs(shootDiagonalMidDown[i].rec, enemy[j].rec))
							{
								shootDiagonalMidDown[i].active = false;
								enemy[j].rec.x = GetRandomValue(screenWidth, screenWidth + 1000);
								enemy[j].rec.y = GetRandomValue(0, screenHeight - enemy[j].rec.height);
								shootRate = 0;
								enemiesKill++;
								score += 100;
							}

							if (shootDiagonalMidDown[i].rec.x + shootDiagonalMidDown[i].rec.width >= screenWidth)
							{
								shootDiagonalMidDown[i].active = false;
								shootRate = 0;
							}
						}
					}
				}
			}
			//mid top
			for (int i = 0; i < NUM_SHOOTS; i++)
			{
				if (shootDiagonalMidTop[i].active)
				{
					// Movement
					shootDiagonalMidTop[i].rec.x += shootDiagonalMidTop[i].speed.x;
					shootDiagonalMidTop[i].rec.y -= shootDiagonalMidTop[i].speed.y / 4;

					// Collision with enemy
					for (int j = 0; j < activeEnemies; j++)
					{
						if (enemy[j].active)
						{
							if (CheckCollisionRecs(shootDiagonalMidTop[i].rec, enemy[j].rec))
							{
								shootDiagonalMidTop[i].active = false;
								enemy[j].rec.x = GetRandomValue(screenWidth, screenWidth + 1000);
								enemy[j].rec.y = GetRandomValue(0, screenHeight - enemy[j].rec.height);
								shootRate = 0;
								enemiesKill++;
								score += 100;
							}

							if (shootDiagonalMidTop[i].rec.x + shootDiagonalMidTop[i].rec.width >= screenWidth)
							{
								shootDiagonalMidTop[i].active = false;
								shootRate = 0;
							}
						}
					}
				}
			}
		}

		void DrawGame()
		{
			ClearBackground(GetColor(0x052c46ff));

			DrawTextureEx(background, Vector2({ scrollingBack, 0 }), 0.0f, 2.0f, WHITE);
			DrawTextureEx(background, Vector2({ background.width * 2 + scrollingBack, 20 }), 0.0f, 2.0f, WHITE);

			DrawTextureEx(midground, Vector2({ scrollingMid, 0 }), 0.0f, 2.0f, WHITE);
			DrawTextureEx(midground, Vector2({ midground.width * 2 + scrollingMid, 20 }), 0.0f, 2.0f, WHITE);

			DrawTextureEx(foreground, Vector2({ scrollingFore, 70 }), 0.0f, 2.0f, WHITE);
			DrawTextureEx(foreground, Vector2({ foreground.width * 2 + scrollingFore, 70 }), 0.0f, 2.0f, WHITE);

			if (!gameOver)
			{

				if (dash)
				{
					BeginBlendMode(blending);
					for (int i = 0; i < MAX_PARTICLES; i++)
					{
						if (mouseTail[i].active) DrawTexturePro(smoke, Rectangle({ 0.0f, 0.0f, (float)smoke.width, (float)smoke.height }),
							Rectangle({
								mouseTail[i].position.x, mouseTail[i].position.y, smoke.width*mouseTail[i].size, smoke.height*mouseTail[i].size
								}),
							Vector2({
								(float)(smoke.width*mouseTail[i].size / 2.0f), (float)(smoke.height*mouseTail[i].size / 2.0f)
								}), mouseTail[i].rotation,
							Fade(mouseTail[i].color, mouseTail[i].alpha));
					}

					EndBlendMode();
				}
				//DrawRectangleRec(player.rec, player.color);
				if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_UP))
					DrawTexture(shipMovement, player.rec.x, player.rec.y, WHITE);
				else if (IsKeyDown(KEY_SPACE))
					DrawTexture(shipShoot, player.rec.x, player.rec.y, WHITE);
				else
					DrawTexture(ship, player.rec.x, player.rec.y, WHITE);

				if (wave == FIRST) DrawText("FIRST WAVE", screenWidth / 2 - MeasureText("FIRST WAVE", 40) / 2, screenHeight / 2 - 40, 40, Fade(RED, alpha));
				else if (wave == SECOND) DrawText("SECOND WAVE", screenWidth / 2 - MeasureText("SECOND WAVE", 40) / 2, screenHeight / 2 - 40, 40, Fade(RED, alpha));
				else if (wave == THIRD) DrawText("THIRD WAVE", screenWidth / 2 - MeasureText("THIRD WAVE", 40) / 2, screenHeight / 2 - 40, 40, Fade(RED, alpha));

				for (int i = 0; i < activeEnemies; i++)
				{
					if (enemy[i].active) DrawTexturePro(meteor, enemy[i].sourceRec, enemy[i].destRec, enemy[i].origin, 0.0f, WHITE);
				}
				for (int i = 0; i < activePU; i++)
				{
					if (powerUp[i].active) DrawTexturePro(po, powerUp[i].sourceRec, powerUp[i].destRec, powerUp[i].origin, 0.0f, WHITE);
				}
				for (int i = 0; i < NUM_SHOOTS; i++)
				{
					if (shoot[i].active) DrawRectangleRec(shoot[i].rec, shoot[i].color);
					if (shootDiagonalTop[i].active)// DrawRectangleRec(shootDiagonalTop[i].rec, shootDiagonalTop[i].color);
						DrawRectanglePro(shootDiagonalTop[i].rec, Vector2({ 0.0f,0.0f }), -45.0f, shootDiagonalTop[i].color);
					if (shootDiagonalDown[i].active)// DrawRectangleRec(shootDiagonalTop[i].rec, shootDiagonalTop[i].color);
						DrawRectanglePro(shootDiagonalDown[i].rec, Vector2({ 0.0f,0.0f }), 45.0f, shootDiagonalDown[i].color);
					if (shootDiagonalMidTop[i].active)// DrawRectangleRec(shootDiagonalTop[i].rec, shootDiagonalTop[i].color);
						DrawRectanglePro(shootDiagonalMidTop[i].rec, Vector2({ 0.0f,0.0f }), -27.5f, shootDiagonalMidTop[i].color);
					if (shootDiagonalMidDown[i].active)// DrawRectangleRec(shootDiagonalTop[i].rec, shootDiagonalTop[i].color);
						DrawRectanglePro(shootDiagonalMidDown[i].rec, Vector2({ 0.0f,0.0f }), 27.5f, shootDiagonalMidDown[i].color);
				}

				DrawText(FormatText("%04i", score), 20, 20, 40, GRAY);
				DrawText(FormatText("%03i", static_cast<int>(mana)), 720, 20, 40, SKYBLUE);
				if (victory) DrawText("YOU WIN", screenWidth / 2 - MeasureText("YOU WIN", 40) / 2, screenHeight / 2 - 40, 40, BLACK);

				if (pause) DrawText("GAME PAUSED", screenWidth / 2 - MeasureText("GAME PAUSED", 40) / 2, screenHeight / 2 - 40, 40, GRAY);


			}
			else
				DrawText("La comiste BB", screenWidth / 2 - MeasureText("GAME PAUSED", 40) / 2, screenHeight / 2 - 40, 40, GRAY);
		}

		void UnloadTexture()
		{
			UnloadTexture(background);
			UnloadTexture(midground);
			UnloadTexture(foreground);
			UnloadTexture(ship);
			UnloadTexture(shipMovement);
			UnloadTexture(shipShoot);

		}
	}

}