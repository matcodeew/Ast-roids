#include <SFML/Graphics.h>
#include <SFML/Window.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#define PI 3.1415926535
#define TOTAL_ASTEROIDS 10
#define MAX_ASTEROIDS 500
#define SPEED_ASTEROIDS 5


int spaceKeyPressed = 0;

int PointVictoire = 30000;

typedef struct Player
{
	int IsAlive;
	float x;
	float y;
	float currentx;
	float currenty;
	int rotation;
	float speed;
	int Score;

} Player;

typedef struct Projectile
{
	float ProjectileX;
	float ProjectileY;
	float projectileSpeed;
	float projectileSpeedX;
	float projectileSpeedY;
	float projectileDirectionX;
	float projectileDirectionY;
	int distanceProjectile;
} Projectile;

typedef struct Enemmis
{
	float X[MAX_ASTEROIDS];
	float Y[MAX_ASTEROIDS];
	float randomDirection[MAX_ASTEROIDS];
	sfSprite* AsteoridsSprite[MAX_ASTEROIDS];
	int VieAsteroids[MAX_ASTEROIDS];
} Enemmis;

typedef struct Sprites
{
	int SpaceShipSpriteWidth;
	int SpaceShipSpriteHeight;
	int AsteoridsSpriteWidth;
	int AsteoridsSpriteHeight;
	int ProjectileSpriteWidth;
	int ProjectileSpriteHeight;
} Sprites;

sfSprite* SpaceShipSprite;
sfSprite* projectileSprite;
sfTexture* AsteoridsTexture;
sfSprite* ExitSprite;
sfFont* fontScore;
sfText* ScoreText;
sfFont* GameOverfont;
sfText* GameOverText;
sfFont* Victoirefont;
sfText* VictoireText;

sfRenderWindow* window;
sfVideoMode videomode = { 1950, 1100, 32 };

void initProjectile(Projectile* projectile, Player* player);
void UpdateProjectile(Projectile* projectile, Player* player);
void Sprite(sfRenderWindow* window, Player* player, Enemmis* enemmis, Sprites* Sprites);
void Deplacement(Player* player);
void UpdateAsteroidsPosition(Enemmis* enemmis, Sprites* Sprites);
void checkCollisions(Player* player, Enemmis* enemmis, Projectile* projectile);
void resetAsteroid(Enemmis* enemmis, int index, Sprites* Sprites);
void GameOver(sfRenderWindow* window, Player* player, sfEvent* event);
void printscore(sfRenderWindow* window, Player* player);
void Victoire(sfRenderWindow* window, Player* player, sfEvent* event);

static float distance(sfVector2f point1, sfVector2f point2)
{
	float dx = point1.x - point2.x;
	float dy = point1.y - point2.y;
	return sqrt(dx * dx + dy * dy);
}

int asteroidsDestroyed = 0;


int main()
{
	Player player;
	player.IsAlive = 3;
	player.x = 0;
	player.y = 0;
	player.rotation = 0;
	player.currentx = 1920 / 2;
	player.currenty = 1080 / 2;
	player.speed = 0.0f;
	player.Score = 0.0f;

	Projectile projectile;
	projectile.ProjectileX = 0;
	projectile.ProjectileY = 0;
	projectile.distanceProjectile = 50;
	projectile.projectileSpeed = 10.0f;
	projectile.projectileSpeedX = 0.0f;
	projectile.projectileSpeedY = 0.0f;
	projectile.projectileDirectionX = 0.0f;
	projectile.projectileDirectionY = 0.0f;

	Enemmis enemmis;
	for (int i = 0; i < MAX_ASTEROIDS; ++i)
	{
		enemmis.X[i] = -20;
		enemmis.Y[i] = -20;
		enemmis.randomDirection[i] = rand() % 360;
		enemmis.AsteoridsSprite[i] = sfSprite_create();
		enemmis.VieAsteroids[i] = 5;
	}

	Sprites Sprites;
	Sprites.SpaceShipSpriteWidth = 32;
	Sprites.SpaceShipSpriteHeight = 32;
	Sprites.AsteoridsSpriteHeight = 64;
	Sprites.AsteoridsSpriteWidth = 64;
	Sprites.ProjectileSpriteHeight = 15;
	Sprites.ProjectileSpriteWidth = 15;

	sfEvent event;
	window = sfRenderWindow_create(videomode, "ASTEROID", sfClose, NULL);
	Sprite(window, &player, &enemmis, &Sprites);
	sfRenderWindow_setFramerateLimit(window, 60);

	// Set du score sur l'écran du jeu
	fontScore = sfFont_createFromFile("police/arial.ttf");
	ScoreText = sfText_create();
	sfText_setFont(ScoreText, fontScore);
	sfText_setCharacterSize(ScoreText, 25);
	sfVector2f position = { 0.0f, 0.0f };
	sfText_setPosition(ScoreText, position);

	// Set du texte sur l'écran de game over
	GameOverfont = sfFont_createFromFile("police/arial.ttf");
	GameOverText = sfText_create();
	sfText_setFont(GameOverText, GameOverfont);
	sfText_setCharacterSize(GameOverText, 150);
	sfText_setColor(GameOverText, sfRed);
	sfVector2f positionGO = { 500.0f, 0.0f };
	sfText_setPosition(GameOverText, positionGO);

	// Set du texte sur l'écran de game over
	Victoirefont = sfFont_createFromFile("police/arial.ttf");
	VictoireText = sfText_create();
	sfText_setFont(VictoireText, Victoirefont);
	sfText_setCharacterSize(VictoireText, 150);
	sfText_setColor(VictoireText, sfYellow);
	sfVector2f positionVictoire = { 600.0f, 0.0f };
	sfText_setPosition(VictoireText, positionVictoire);

	while (sfRenderWindow_isOpen(window))
	{
		if (sfRenderWindow_pollEvent(window, &event))
		{
			if (event.type == sfEvtClosed)
				sfRenderWindow_close(window);
		}
		if (player.IsAlive == 0)
		{
			GameOver(window, &player, &event);
		}
		else
		{
			if (player.Score >= PointVictoire)
			{
				Victoire(window, &player, &event);
			}
			else
			{
				int activeAsteroids = 0;
				for (int i = 0; i < MAX_ASTEROIDS; ++i)
				{
					if (enemmis.VieAsteroids[i] == 0)
					{
						activeAsteroids++;
					}
				}
				if (activeAsteroids < TOTAL_ASTEROIDS)
				{
					for (int i = 0; i < MAX_ASTEROIDS; ++i)
					{
						if (enemmis.VieAsteroids[i] <= 0)
						{
							asteroidsDestroyed++;

							if (asteroidsDestroyed >= 5)
							{
								asteroidsDestroyed = 0;
								resetAsteroid(&enemmis, i, &Sprites);
							}
						}
					}
				}
				checkCollisions(&player, &enemmis, &projectile);

				if (sfKeyboard_isKeyPressed(sfKeySpace) && spaceKeyPressed == 0)
				{
					initProjectile(&projectile, &player);
				}

				if (spaceKeyPressed > 0)
				{
					UpdateProjectile(&projectile, &player);
				}

				sfVector2f position = sfSprite_getPosition(SpaceShipSprite);
				position.x += player.x;
				position.y += player.y;

				// Vérifiez si le vaisseau sort de l'écran et réapparaissez de l'autre côté.
				if (position.x > videomode.width)
				{
					position.x = -Sprites.SpaceShipSpriteWidth;
				}
				else if (position.x < -Sprites.SpaceShipSpriteWidth)
				{
					position.x = videomode.width;
				}

				if (position.y > videomode.height)
				{
					position.y = -Sprites.SpaceShipSpriteHeight;
				}
				else if (position.y < -Sprites.SpaceShipSpriteHeight)
				{
					position.y = videomode.height;
				}
				sfSprite_setPosition(SpaceShipSprite, position);
				player.currentx = position.x;
				player.currenty = position.y;
				sfRenderWindow_clear(window, sfBlack);

				for (int i = 0; i < MAX_ASTEROIDS; ++i)
				{
					sfRenderWindow_drawSprite(window, enemmis.AsteoridsSprite[i], NULL);
				}
				sfRenderWindow_drawSprite(window, SpaceShipSprite, NULL);
				sfRenderWindow_drawSprite(window, projectileSprite, NULL);
				printscore(window, &player);
				sfRenderWindow_display(window);

				Deplacement(&player);
				UpdateAsteroidsPosition(&enemmis, &Sprites);
			}
		}
	}

	sfRenderWindow_destroy(window);
	sfSprite_destroy(SpaceShipSprite);
	sfSprite_destroy(projectileSprite);
	for (int i = 0; i < MAX_ASTEROIDS; ++i)
	{
		sfSprite_destroy(enemmis.AsteoridsSprite[i]);
	}

	return 0;
}


void initProjectile(Projectile* projectile, Player* player)
{
	projectile->distanceProjectile = 50;
	// Calculez les composantes x et y de la vitesse du projectile en fonction de la rotation du vaisseau
	projectile->projectileDirectionX = cos(player->rotation * PI / 180);
	projectile->projectileDirectionY = sin(player->rotation * PI / 180);
	projectile->ProjectileX = player->currentx + 50 * cos(player->rotation * PI / 180);
	projectile->ProjectileY = player->currenty + 50 * sin(player->rotation * PI / 180);
	sfSprite_setPosition(projectileSprite, (sfVector2f) { projectile->ProjectileX, projectile->ProjectileY });
	sfSprite_setScale(projectileSprite, (sfVector2f) { 1.1, 1.1 });
	spaceKeyPressed = 1;
}

void UpdateProjectile(Projectile* projectile, Player* player)
{
	// Calculez les composantes x et y de la vitesse en fonction de la rotation du vaisseau
	projectile->projectileSpeedX = (projectile->projectileSpeed * player->speed) * cos(player->rotation * PI / 180);
	projectile->projectileSpeedY = (projectile->projectileSpeed * player->speed) * sin(player->rotation * PI / 180);

	if (projectile->distanceProjectile <= 50 && projectile->distanceProjectile > -1)
	{
		// Mettez à jour la position du projectile en fonction de la vitesse calculée
		projectile->ProjectileX += projectile->projectileSpeed * projectile->projectileDirectionX;
		projectile->ProjectileY += projectile->projectileSpeed * projectile->projectileDirectionY;
		sfSprite_setPosition(projectileSprite, (sfVector2f) { projectile->ProjectileX, projectile->ProjectileY });
		projectile->distanceProjectile--;
	}

	if (projectile->distanceProjectile < 0)
	{
		spaceKeyPressed = 0;
		projectile->ProjectileX = -1000;
		projectile->ProjectileY = -1000;
		sfSprite_setPosition(projectileSprite, (sfVector2f) { projectile->ProjectileX, projectile->ProjectileY });
	}
}

void Sprite(sfRenderWindow* window, Player* player, Enemmis* enemmis, Sprites* Sprites)
{
	sfTexture* SpaceShipTexture = sfTexture_createFromFile("Textures/space_ship.png", NULL);
	SpaceShipSprite = sfSprite_create();
	sfVector2f playercord = { player->currentx, player->currenty };
	sfVector2f sOrigin = { 32.0f, 32.0f };
	sfSprite_setOrigin(SpaceShipSprite, sOrigin);
	sfSprite_setTexture(SpaceShipSprite, SpaceShipTexture, sfFalse);
	sfSprite_setPosition(SpaceShipSprite, playercord);
	sfSprite_setScale(SpaceShipSprite, (sfVector2f) { 1.1, 1.1 });

	sfTexture* ProjectileTexture = sfTexture_createFromFile("Textures/projectile.png", NULL);
	projectileSprite = sfSprite_create();
	sfVector2f playercordPR = { -1000.0f, -1000.0f };
	sfVector2f sOriginPR = { 5.0f, 5.0f };
	sfSprite_setOrigin(projectileSprite, sOriginPR);
	sfSprite_setTexture(projectileSprite, ProjectileTexture, sfFalse);
	sfSprite_setPosition(projectileSprite, playercordPR);
	sfSprite_setScale(projectileSprite, (sfVector2f) { 1.1, 1.1 });

	sfTexture* ExitTexture = sfTexture_createFromFile("Textures/exit.png", NULL);
	ExitSprite = sfSprite_create();
	sfVector2f playercordExit = { 1100.0f, 900.0f };
	sfVector2f sOriginExit = { 300.0f, 100.0f };
	sfSprite_setOrigin(ExitSprite, sOriginExit);
	sfSprite_setTexture(ExitSprite, ExitTexture, sfFalse);
	sfSprite_setPosition(ExitSprite, playercordExit);
	sfSprite_setScale(ExitSprite, (sfVector2f) { 1.1, 1.1 });


	AsteoridsTexture = sfTexture_createFromFile("Textures/asteroides.png", NULL);

	for (int i = 0; i < MAX_ASTEROIDS; ++i)
	{
		sfVector2f enemmisCord = { enemmis->X[i], enemmis->Y[i] };
		sfVector2f sOrigin1 = { 32.0f, 32.0f };
		sfSprite_setOrigin(enemmis->AsteoridsSprite[i], sOrigin1);
		sfSprite_setTexture(enemmis->AsteoridsSprite[i], AsteoridsTexture, sfFalse);
		sfSprite_setPosition(enemmis->AsteoridsSprite[i], enemmisCord);
		sfSprite_setScale(enemmis->AsteoridsSprite[i], (sfVector2f) { 3.0, 3.0 });
	}
}

void Deplacement(Player* player)
{
	if (player->IsAlive >= 1)
	{
		if (sfKeyboard_isKeyPressed(sfKeyZ))
		{
			if (player->speed < 15)
			{
				player->speed += 0.2;
			}
			player->currentx += player->speed * cos(player->rotation * PI / 180);
			player->currenty += player->speed * sin(player->rotation * PI / 180);
			sfSprite_setPosition(SpaceShipSprite, (sfVector2f) { player->currentx, player->currenty });
		}
		// permet la décélération du vaisseau et remise a zero de la vitesse.
		else
		{
			if (player->speed > 0)
			{
				player->speed -= 0.1;
			}
			player->currentx += player->speed * cos(player->rotation * PI / 180);
			player->currenty += player->speed * sin(player->rotation * PI / 180);
			sfSprite_setPosition(SpaceShipSprite, (sfVector2f) { player->currentx, player->currenty });
		}

		if (sfKeyboard_isKeyPressed(sfKeyD))
		{
			player->rotation += 7;
			sfSprite_setRotation(SpaceShipSprite, player->rotation);
		}
		else if (sfKeyboard_isKeyPressed(sfKeyQ))
		{
			player->rotation -= 7;
			sfSprite_setRotation(SpaceShipSprite, player->rotation);
		}
	}
}

void UpdateAsteroidsPosition(Enemmis* enemmis, Sprites* Sprites)
{

	for (int i = 0; i < MAX_ASTEROIDS; ++i)
	{

		sfVector2f asteroidPos = sfSprite_getPosition(enemmis->AsteoridsSprite[i]);
		asteroidPos.x += SPEED_ASTEROIDS * cos(enemmis->randomDirection[i] * PI / 180);
		asteroidPos.y += SPEED_ASTEROIDS * sin(enemmis->randomDirection[i] * PI / 180);

		// Vérifiez si l'astéroïde sort de l'écran et faites-le réapparaître de l'autre côté si nécessaire

		//bord droite
		if (asteroidPos.x > videomode.width + 50)
		{
			asteroidPos.x = -Sprites->AsteoridsSpriteWidth;
			enemmis->randomDirection[i] += rand() % 90;
		}
		//bord gauche
		else if (asteroidPos.x < -Sprites->AsteoridsSpriteWidth - 50)
		{
			asteroidPos.x = videomode.width;
			enemmis->randomDirection[i] += rand() % 90;
		}
		//bord bas
		if (asteroidPos.y > videomode.height + 50)
		{
			asteroidPos.y = -Sprites->AsteoridsSpriteHeight;
			enemmis->randomDirection[i] += rand() % 90;
		}
		//bord haut
		else if (asteroidPos.y < -Sprites->AsteoridsSpriteHeight - 50)
		{
			asteroidPos.y = videomode.height;
			enemmis->randomDirection[i] += rand() % 90;
		}
		sfSprite_setPosition(enemmis->AsteoridsSprite[i], asteroidPos);
	}
}

void checkCollisions(Player* player, Enemmis* enemmis, Projectile* projectile)
{
	sfTexture* Phase1Texture = sfTexture_createFromFile("Textures/asteroides.png", NULL);
	sfTexture* Phase2Texture = sfTexture_createFromFile("Textures/asteroidesFissure.png", NULL);

	sfVector2f projectilePos = sfSprite_getPosition(projectileSprite);
	sfFloatRect projectileBounds = sfSprite_getGlobalBounds(projectileSprite);

	sfVector2f PlayerPos = sfSprite_getPosition(SpaceShipSprite);
	sfFloatRect PlayerBounds = sfSprite_getGlobalBounds(SpaceShipSprite);
	for (int i = 0; i < MAX_ASTEROIDS; ++i)
	{
		sfVector2f asteroidPos = sfSprite_getPosition(enemmis->AsteoridsSprite[i]);
		float asteroidRadius = 32 * 1.5;

		if (distance(asteroidPos, projectilePos) < asteroidRadius + projectileBounds.width)
		{
			projectile->ProjectileX = -1000;
			projectile->ProjectileY = -1000;
			sfSprite_setPosition(projectileSprite, (sfVector2f) { projectile->ProjectileX, projectile->ProjectileY });
			projectile->distanceProjectile = 0;
			enemmis->VieAsteroids[i]--;

			// Si la vie de l'astéroïde est épuisée, changez la texture
			if (enemmis->VieAsteroids[i] == 3)
			{
				sfSprite_setTexture(enemmis->AsteoridsSprite[i], Phase2Texture, sfTrue);
			}
			if (enemmis->VieAsteroids[i] == 2)
			{
				sfSprite_setScale(enemmis->AsteoridsSprite[i], (sfVector2f) { 2.0, 2.0 });
				sfSprite_setTexture(enemmis->AsteoridsSprite[i], Phase1Texture, sfTrue);
			}
			if (enemmis->VieAsteroids[i] == 1)
			{
				sfSprite_setScale(enemmis->AsteoridsSprite[i], (sfVector2f) { 1.0, 1.0 });
				sfSprite_setTexture(enemmis->AsteoridsSprite[i], Phase2Texture, sfTrue);
			}
			if (enemmis->VieAsteroids[i] == 0)
			{
				sfSprite_setScale(enemmis->AsteoridsSprite[i], (sfVector2f) { 0.0, 0.0 });
				sfSprite_setTexture(enemmis->AsteoridsSprite[i], Phase1Texture, sfTrue);
				asteroidsDestroyed++;
				player->Score += 2000;
			}
		}

		if (distance(asteroidPos, PlayerPos) < asteroidRadius + PlayerBounds.width)
		{
			player->IsAlive = 0;
		}
	}
}

void resetAsteroid(Enemmis* enemmis, int index, Sprites* Sprites) {
	enemmis->X[index] = -20;
	enemmis->Y[index] = -20;
	enemmis->randomDirection[index] = rand() % 360;
	enemmis->AsteoridsSprite[index] = sfSprite_create();
	enemmis->VieAsteroids[index] = 5;

	sfTexture* asteroidTexture = sfTexture_createFromFile("Textures/asteroides.png", NULL);
	sfSprite_setTexture(enemmis->AsteoridsSprite[index], asteroidTexture, sfFalse);
	sfSprite_setOrigin(enemmis->AsteoridsSprite[index], (sfVector2f) { Sprites->AsteoridsSpriteWidth / 2.0f, Sprites->AsteoridsSpriteHeight / 2.0f });
	sfSprite_setPosition(enemmis->AsteoridsSprite[index], (sfVector2f) { enemmis->X[index], enemmis->Y[index] });
	sfSprite_setScale(enemmis->AsteoridsSprite[index], (sfVector2f) { 3.0, 3.0 });
}

void GameOver(sfRenderWindow* window, Player* player, sfEvent* event)
{

	sfVector2i mouse_position = sfMouse_getPositionRenderWindow(window);
	int mouseX = mouse_position.x;
	int mouseY = mouse_position.y;
	printf("Mouse X: %d, Mouse Y: %d\n", mouseX, mouseY);

	if (mouseX >= 770 && mouseX <= 1100 && mouseY >= 788 && mouseY <= 900)
	{
		if (event->type == sfEvtMouseButtonPressed && event->mouseButton.button == sfMouseLeft)
		{
			sfRenderWindow_close(window);
		}
	}

	char GOString[50];
	snprintf(GOString, sizeof(GOString), "GAME OVER");
	sfText_setString(GameOverText, GOString);
	sfRenderWindow_clear(window, sfBlack);
	sfRenderWindow_drawText(window, GameOverText, NULL);
	sfRenderWindow_drawSprite(window, ExitSprite, NULL);
	sfRenderWindow_display(window);
}

void printscore(sfRenderWindow* window, Player* player)
{
	if (player->IsAlive >= 1)
	{
		char scoreString[50];
		snprintf(scoreString, sizeof(scoreString), " Score: %d", player->Score);
		sfText_setString(ScoreText, scoreString);
		sfRenderWindow_drawText(window, ScoreText, NULL);
	}
}

void Victoire(sfRenderWindow* window, Player* player, sfEvent* event)
{
	sfVector2i mouse_position = sfMouse_getPositionRenderWindow(window);
	int mouseX = mouse_position.x;
	int mouseY = mouse_position.y;
	printf("Mouse X: %d, Mouse Y: %d\n", mouseX, mouseY);

	if (mouseX >= 770 && mouseX <= 1100 && mouseY >= 788 && mouseY <= 900)
	{
		if (event->type == sfEvtMouseButtonPressed && event->mouseButton.button == sfMouseLeft)
		{
			sfRenderWindow_close(window);
		}
	}
	char GOString[50];
	snprintf(GOString, sizeof(GOString), "VICTOIRE");
	sfText_setString(VictoireText, GOString);
	sfRenderWindow_clear(window, sfBlack);
	sfRenderWindow_drawText(window, VictoireText, NULL);
	sfRenderWindow_drawSprite(window, ExitSprite, NULL);
	sfRenderWindow_display(window);
}

