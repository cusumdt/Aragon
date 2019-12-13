#include "gameplay.h"

#include "Game\game.h"

namespace Game
{
	namespace Gameplay
	{

		static const int NUM_MAX_ENEMIES = 50;
		static const int FIRST_WAVE = 30;
		static const int SECOND_WAVE = 40;
		static const int THIRD_WAVE = 50;
		static const int MAX_PARTICLES = 200;
		static const int DASH_COST = 20;
		static const int LIMIT_MANA = 100;
		static const int NUM_PU = 2;
		static const int NUM_SHOOTS = 50;
		static const int ADD_SCORE_MIN = 100;
		static const int ADD_SCORE_MAX = 100000;
		static const int ADD_SCORE_MID = 10000;
		static const int LIMIT_POS = 1000;
		static const int SPEED_BULLET = 7;
		// Particle structure with basic data

		struct Particle {
			Vector2 position;
			Color color;
			float alpha;
			float size;
			float rotation;
			bool active;        // NOTE: Use it to activate/deactive particle
		};

		enum EnemyWave { FIRST = 0, SECOND, THIRD } ;

		struct Enemy {
			Rectangle rec;
			Vector2 speed;
			bool active;
			Color color;
			Rectangle sourceRec;
			Rectangle destRec;
			Vector2 origin;
		};

		struct PowerUp {
			Rectangle rec;
			Vector2 speed;
			bool active;
			Color color;
			Rectangle sourceRec;
			Rectangle destRec;
			Vector2 origin;
		};

		struct Shoot {
			Rectangle rec;
			Vector2 speed;
			bool active;
			Color color;
		};

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
		static Texture2D ship;
		static Texture2D shipMovement;
		static Texture2D shipShoot;
		static Texture2D background;
		static Texture2D midground;
		static Texture2D foreground;
		static Texture2D meteor;
		static Texture2D po;


		static int currentFrame = 0;
		static int framesCounter = 0;
		static int framesSpeed = 8;
		static float scrollingBack = 0.0f;
		static float scrollingMid = 0.0f;
		static float scrollingFore = 0.0f;
		static bool dash = false;
		static float time = 0;
		static Particle mouseTail[MAX_PARTICLES] = { 0 };
		static float gravity;
		static Texture2D smoke;
		static int blending;
		static Vector2 positionExplo;
		static bool activeExplo = false;
		static float mana;
		static int powerUpTier;
		
		void initGame()
		{
			mana = LIMIT_MANA;
			// Initialize particles
			for (int i = 0; i < MAX_PARTICLES; i++)
			{
				mouseTail[i].position = Vector2({ 0, 0 });
				mouseTail[i].color = WHITE;
				mouseTail[i].alpha = 1.0f;
				mouseTail[i].size = static_cast<float>(GetRandomValue(1, 30)) / 20.0f;
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
				enemy[i].rec.x = static_cast<float>(GetRandomValue(screenWidth, screenWidth + LIMIT_POS));
				enemy[i].rec.y = static_cast<float>(GetRandomValue(0, static_cast<int>(screenHeight - enemy[i].rec.height)));
				enemy[i].speed.x = 5;
				enemy[i].speed.y = 5;
				enemy[i].active = true;
				enemy[i].color = GRAY;
				enemy[i].sourceRec = { 0.0f,0.0f,static_cast<float>(meteor.width),static_cast<float>(meteor.height) };
				enemy[i].destRec = { enemy[i].rec.x,enemy[i].rec.y,static_cast<float>(meteor.width),static_cast<float>(meteor.height) };
				enemy[i].origin = { static_cast<float>(meteor.width) / 2,static_cast<float>(meteor.height) / 2 };
			}
			//------------------------
			// Initialize Power UPS
			for (int i = 0; i < NUM_PU; i++)
			{
				powerUp[i].rec.width = 40;
				powerUp[i].rec.height = 40;
				powerUp[i].rec.x = static_cast<float>(GetRandomValue(screenWidth, screenWidth + LIMIT_POS));
				powerUp[i].rec.y = static_cast<float>(GetRandomValue(0, static_cast<int>(screenHeight -  powerUp[i].rec.height)));
				powerUp[i].speed.x = 5;
				powerUp[i].speed.y = 5;
				powerUp[i].active = true;
				powerUp[i].color = GRAY;
				powerUp[i].sourceRec = { 0.0f,0.0f,static_cast<float>(meteor.width),static_cast<float>(meteor.height) };
				powerUp[i].destRec = { enemy[i].rec.x,enemy[i].rec.y,static_cast<float>(meteor.width),static_cast<float>(meteor.height) };
				powerUp[i].origin = { static_cast<float>(meteor.width / 2),static_cast<float>(meteor.height / 2)};
			}
			// Initialize shoots
			for (int i = 0; i < NUM_SHOOTS; i++)
			{
				shoot[i].rec.x = player.rec.x;
				shoot[i].rec.y = player.rec.y + player.rec.height / 4;
				shoot[i].rec.width = 10;
				shoot[i].rec.height = 5;
				shoot[i].speed.x = SPEED_BULLET;
				shoot[i].speed.y = 0;
				shoot[i].active = false;
				shoot[i].color = RAYWHITE;
				shootDiagonalTop[i].rec.x = player.rec.x;
				shootDiagonalTop[i].rec.y = player.rec.y + player.rec.height / 4;
				shootDiagonalTop[i].rec.width = 10;
				shootDiagonalTop[i].rec.height = 5;
				shootDiagonalTop[i].speed.x = SPEED_BULLET;
				shootDiagonalTop[i].speed.y = SPEED_BULLET;
				shootDiagonalTop[i].active = false;
				shootDiagonalTop[i].color = RAYWHITE;
				shootDiagonalDown[i].rec.x = player.rec.x;
				shootDiagonalDown[i].rec.y = player.rec.y + player.rec.height / 4;
				shootDiagonalDown[i].rec.width = 10;
				shootDiagonalDown[i].rec.height = 5;
				shootDiagonalDown[i].speed.x = SPEED_BULLET;
				shootDiagonalDown[i].speed.y = SPEED_BULLET;
				shootDiagonalDown[i].active = false;
				shootDiagonalDown[i].color = RAYWHITE;
				shootDiagonalMidTop[i].rec.x = player.rec.x;
				shootDiagonalMidTop[i].rec.y = player.rec.y + player.rec.height / 4;
				shootDiagonalMidTop[i].rec.width = 10;
				shootDiagonalMidTop[i].rec.height = 5;
				shootDiagonalMidTop[i].speed.x = SPEED_BULLET;
				shootDiagonalMidTop[i].speed.y = SPEED_BULLET;
				shootDiagonalMidTop[i].active = false;
				shootDiagonalMidTop[i].color = RAYWHITE;
				shootDiagonalMidDown[i].rec.x = player.rec.x;
				shootDiagonalMidDown[i].rec.y = player.rec.y + player.rec.height / 4;
				shootDiagonalMidDown[i].rec.width = 10;
				shootDiagonalMidDown[i].rec.height = 5;
				shootDiagonalMidDown[i].speed.x = SPEED_BULLET;
				shootDiagonalMidDown[i].speed.y = SPEED_BULLET;
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
				else
				{
					if (IsKeyPressed(KEY_ENTER))
					{
						gameOver = false;
						firstInit = true;
						activePU = 0;
						powerUpTier = 0;
						screen = MENU;
					}
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
			scrollingBack -= 0.1f * 100 * GetFrameTime();
			scrollingMid -= 0.5f * 100 * GetFrameTime();
			scrollingFore -= 1.0f * 100 * GetFrameTime();
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
				score += ADD_SCORE_MAX;
			}

		}

		void PlayerActions()
		{
			// Player movement
			if (IsKeyDown(KEY_RIGHT)) player.rec.x += player.speed.x  * 100 * GetFrameTime();
			if (IsKeyDown(KEY_LEFT)) player.rec.x -= player.speed.x * 100 * GetFrameTime();
			if (IsKeyDown(KEY_UP)) player.rec.y -= player.speed.y * 100 * GetFrameTime();
			if (IsKeyDown(KEY_DOWN)) player.rec.y += player.speed.y * 100 * GetFrameTime();
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
							enemy[i].rec.x = static_cast<float>(GetRandomValue(screenWidth, screenWidth + LIMIT_POS));
							enemy[i].rec.y = static_cast<float>(GetRandomValue(0, static_cast<int>(screenHeight - enemy[i].rec.height)));
							enemiesKill++;
							score += ADD_SCORE_MIN;
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
						score += ADD_SCORE_MID;
						powerUp[i].rec.x = static_cast<float>(GetRandomValue(screenWidth, screenWidth + LIMIT_POS));
						powerUp[i].rec.y = static_cast<float>(GetRandomValue(0, static_cast<int>(screenHeight - powerUp[i].rec.height)));
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
					enemy[i].rec.x -= enemy[i].speed.x * 50 * GetFrameTime();

					if (enemy[i].rec.x < 0)
					{
						enemy[i].rec.x = static_cast<float>(GetRandomValue(screenWidth, screenWidth + LIMIT_POS));
						enemy[i].rec.y = static_cast<float>(GetRandomValue(0, static_cast<int>(screenHeight - enemy[i].rec.height)));
					}
					enemy[i].destRec = { enemy[i].rec.x,enemy[i].rec.y,static_cast<float>(meteor.width),static_cast<float>(meteor.height) };
				}
			}

			for (int i = 0; i < activePU; i++)
			{
				if (powerUp[i].active)
				{
					powerUp[i].rec.x -= powerUp[i].speed.x * 50 * GetFrameTime();

					if (powerUp[i].rec.x < 0)
					{
						powerUp[i].rec.x = static_cast<float>(GetRandomValue(screenWidth, screenWidth + LIMIT_POS));
						powerUp[i].rec.y = static_cast<float>(GetRandomValue(0, static_cast<int>(screenHeight - powerUp[i].rec.height)));
					}
					powerUp[i].destRec = { powerUp[i].rec.x,powerUp[i].rec.y,static_cast<float>(po.width),static_cast<float>(po.height) };
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
					shoot[i].rec.x += shoot[i].speed.x * 100 * GetFrameTime();

					// Collision with enemy
					for (int j = 0; j < activeEnemies; j++)
					{
						if (enemy[j].active)
						{
							if (CheckCollisionRecs(shoot[i].rec, enemy[j].rec))
							{
								shoot[i].active = false;
								enemy[j].rec.x = static_cast<float>(GetRandomValue(screenWidth, screenWidth + LIMIT_POS));
								enemy[j].rec.y = static_cast<float>(GetRandomValue(0, static_cast<int>(screenHeight - enemy[j].rec.height)));
								shootRate = 0;
								enemiesKill++;
								score += ADD_SCORE_MIN;
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
					shootDiagonalTop[i].rec.x += shootDiagonalTop[i].speed.x * 100 * GetFrameTime();
					shootDiagonalTop[i].rec.y -= shootDiagonalTop[i].speed.y / 2 * 100 * GetFrameTime();

					// Collision with enemy
					for (int j = 0; j < activeEnemies; j++)
					{
						if (enemy[j].active)
						{
							if (CheckCollisionRecs(shootDiagonalTop[i].rec, enemy[j].rec))
							{
								shootDiagonalTop[i].active = false;
								enemy[j].rec.x = static_cast<float>(GetRandomValue(screenWidth, screenWidth + LIMIT_POS));
								enemy[j].rec.y = static_cast<float>(GetRandomValue(0, static_cast<int>(screenHeight - enemy[j].rec.height)));
								shootRate = 0;
								enemiesKill++;
								score += ADD_SCORE_MIN;
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
					shootDiagonalDown[i].rec.x += shootDiagonalDown[i].speed.x * 100 * GetFrameTime();
					shootDiagonalDown[i].rec.y += shootDiagonalDown[i].speed.y / 2 * 100 * GetFrameTime();

					// Collision with enemy
					for (int j = 0; j < activeEnemies; j++)
					{
						if (enemy[j].active)
						{
							if (CheckCollisionRecs(shootDiagonalDown[i].rec, enemy[j].rec))
							{
								shootDiagonalDown[i].active = false;
								enemy[j].rec.x = static_cast<float>(GetRandomValue(screenWidth, screenWidth + LIMIT_POS));
								enemy[j].rec.y = static_cast<float>(GetRandomValue(0, static_cast<int>(screenHeight - enemy[j].rec.height)));
								shootRate = 0;
								enemiesKill++;
								score += ADD_SCORE_MIN;
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
					shootDiagonalMidDown[i].rec.x += shootDiagonalMidDown[i].speed.x * 100 * GetFrameTime();
					shootDiagonalMidDown[i].rec.y += shootDiagonalMidDown[i].speed.y / 4 * 100 * GetFrameTime();

					// Collision with enemy
					for (int j = 0; j < activeEnemies; j++)
					{
						if (enemy[j].active)
						{
							if (CheckCollisionRecs(shootDiagonalMidDown[i].rec, enemy[j].rec))
							{
								shootDiagonalMidDown[i].active = false;
								enemy[j].rec.x = static_cast<float>(GetRandomValue(screenWidth, screenWidth + LIMIT_POS));
								enemy[j].rec.y = static_cast<float>(GetRandomValue(0, static_cast<int>(screenHeight - enemy[j].rec.height)));
								shootRate = 0;
								enemiesKill++;
								score += ADD_SCORE_MIN;
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
					shootDiagonalMidTop[i].rec.x += shootDiagonalMidTop[i].speed.x * 100 * GetFrameTime();
					shootDiagonalMidTop[i].rec.y -= shootDiagonalMidTop[i].speed.y / 4 * 100 * GetFrameTime();

					// Collision with enemy
					for (int j = 0; j < activeEnemies; j++)
					{
						if (enemy[j].active)
						{
							if (CheckCollisionRecs(shootDiagonalMidTop[i].rec, enemy[j].rec))
							{
								shootDiagonalMidTop[i].active = false;
								enemy[j].rec.x = static_cast<float>(GetRandomValue(screenWidth, screenWidth + LIMIT_POS));
								enemy[j].rec.y = static_cast<float>(GetRandomValue(0, static_cast<int>(screenHeight - enemy[j].rec.height)));
								shootRate = 0;
								enemiesKill++;
								score += ADD_SCORE_MIN;
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

			for (int i = 0; i < activeEnemies; i++)
			{
				if (enemy[i].active) DrawTexturePro(meteor, enemy[i].sourceRec, enemy[i].destRec, enemy[i].origin, 0.0f, WHITE);
			}
			for (int i = 0; i < activePU; i++)
			{
				if (powerUp[i].active) DrawTexturePro(po, powerUp[i].sourceRec, powerUp[i].destRec, powerUp[i].origin, 0.0f, WHITE);
			}
			if (!gameOver)
			{

				if (dash)
				{
					BeginBlendMode(blending);
					for (int i = 0; i < MAX_PARTICLES; i++)
					{
						if (mouseTail[i].active) DrawTexturePro(smoke, Rectangle({ 0.0f, 0.0f, static_cast<float>(smoke.width), static_cast<float>(smoke.height) }),
							Rectangle({
								mouseTail[i].position.x, mouseTail[i].position.y, smoke.width*mouseTail[i].size, smoke.height*mouseTail[i].size
								}),
							Vector2({
								static_cast<float>((smoke.width*mouseTail[i].size / 2.0f)), static_cast<float>((smoke.height*mouseTail[i].size / 2.0f))
								}), mouseTail[i].rotation,
							Fade(mouseTail[i].color, mouseTail[i].alpha));
					}

					EndBlendMode();
				}
				//DrawRectangleRec(player.rec, player.color);
			
				if (wave == FIRST) DrawText("FIRST WAVE", screenWidth / 2 - MeasureText("FIRST WAVE", 40) / 2, screenHeight / 2 - 40, 40, Fade(RED, alpha));
				else if (wave == SECOND) DrawText("SECOND WAVE", screenWidth / 2 - MeasureText("SECOND WAVE", 40) / 2, screenHeight / 2 - 40, 40, Fade(RED, alpha));
				else if (wave == THIRD) DrawText("THIRD WAVE", screenWidth / 2 - MeasureText("THIRD WAVE", 40) / 2, screenHeight / 2 - 40, 40, Fade(RED, alpha));

			
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

				if (pause)
				{
					DrawText("GAME PAUSED", screenWidth / 2 - MeasureText("GAME PAUSED", 40) / 2, screenHeight / 2 - 40, 40, GRAY);
					DrawText("PRESS ENTER TO MENU", screenWidth / 2 - MeasureText("PRESS ENTER TO MENU", 40) / 2, screenHeight / 2 + 40, 40, GRAY);
				}



			}
			else
			{
				DrawText("DEFEAT", screenWidth / 2 - MeasureText("DEFEAT", 40) / 2, screenHeight / 2 - 40, 40, GRAY);
				DrawText("PRESS ENTER", screenWidth / 2 - MeasureText("PRESS ENTER", 40) / 2, screenHeight / 2 + 40, 40, GRAY);
			}

			
			if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_UP))
				DrawTexture(shipMovement, static_cast<int>(player.rec.x), static_cast<int>(player.rec.y), WHITE);
			else if (IsKeyDown(KEY_SPACE))
				DrawTexture(shipShoot, static_cast<int>(player.rec.x), static_cast<int>(player.rec.y), WHITE);
			else
				DrawTexture(ship, static_cast<int>(player.rec.x), static_cast<int>(player.rec.y), WHITE);

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