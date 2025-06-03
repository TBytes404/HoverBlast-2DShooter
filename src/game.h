#pragma once
#include "raylib.h"
#include "ship.h"

#ifdef DEBUG
#include "debug.h"
#endif

Ship enemy, player;
bool pause, gameover;

void resetGame(void) {
  pause = gameover = false;
  initBot(), initTextInput();
  SeekMusicStream(backgroundMusic, 0);
  initShip(&enemy, arenaWidth() / 2., 0, true);
  initShip(&player, arenaWidth() / 2., GetScreenHeight(), false);
}

void calculateBotMovement(Ship *enemy, Ship *player) {
  // avoid bullet hits
  botFrameDelay++;
  if (botFrameDelay > botMaxFrameDelay)
    botFrameDelay = 0, enemy->direc = NONE;
  else if (enemy->direc == UP || enemy->direc == DOWN)
    enemy->direc = NONE;
  else
    for (int i = 0; i < maxShots; i++)
      if (player->shots[i].active) {
        if (player->shots[i].pos.x > enemy->rect.x &&
            player->shots[i].pos.x < (enemy->rect.x + enemy->rect.width))
          enemy->direc =
              (player->shots[i].pos.x - enemy->rect.x) < 0 ? RIGHT : LEFT;
        else if (player->shots[i].active &&
                 player->shots[i].pos.y < arenaHeight())
          enemy->direc = enemy->rect.x < arenaWidth() / 2. ? RIGHT : LEFT;
      }

  // follow player when too far
  if (player->rect.x - enemy->rect.x > botFollowThresh())
    enemy->direc = RIGHT;
  else if (player->rect.x - enemy->rect.x < -botFollowThresh())
    enemy->direc = LEFT;

  // move in random direction
  int rand = GetRandomValue(0, botMoveDirecProba);
  if (rand == 0)
    enemy->direc = GetRandomValue(0, 1) ? UP : DOWN;
  else if (rand == botMoveDirecProba)
    enemy->direc = GetRandomValue(0, 1) ? LEFT : RIGHT;
}

void drawShipStats(Ship *ship) {
  drawStats(ship->isEnemy ? "Bot"
                          : ((nameInput[0] == '\0') ? "Player" : nameInput),
            ship->score, ship->health,
            ship->isEnemy ? fontSize : GetScreenHeight() - fontSize * 4,
            fontSize, ship->isEnemy ? RED : VIOLET);
}

void updateGame(void) {
  UpdateMusicStream(backgroundMusic);

  // game over logic
  if (enemy.health <= 0)
    enemy.alive = false;

  else if (player.health <= 0)
    player.alive = false;

  else if (IsKeyPressed(KEY_LEFT_CONTROL) || IsKeyPressed(KEY_RIGHT_CONTROL))
    pause = true;

  // enemy logic
  calculateBotMovement(&enemy, &player);
  if (enemy.alive) {
    handleMovement(&enemy);
    for (int i = 0; i < maxShots; i++)
      if (checkShot(&player.shots[i], enemy.rect))
        PlaySound(getShotSound), player.score++, enemy.health--;
    shoot(&enemy, (enemy.rect.x + enemy.rect.width) >=
                          player.rect.x && // shoot when player in sight
                      enemy.rect.x <= (player.rect.x + player.rect.width));
  } else
    gameover = overBlast(&enemy);

  // player logic
  if (player.alive) {
    handleMovement(&player);
    for (int i = 0; i < maxShots; i++)
      if (checkShot(&enemy.shots[i], player.rect))
        PlaySound(getShotSound), enemy.score++, player.health--;
    shoot(&player, IsKeyDown(KEY_F) || IsKeyDown(KEY_SPACE));
  } else
    gameover = overBlast(&player);

  // shots
  for (int i = 0; i < maxShots; i++)
    moveShot(&enemy.shots[i]), moveShot(&player.shots[i]);
}

void drawGame(void) {
  // background tiles
  for (int y = 0; y < GetScreenHeight(); y += bgSpan)
    for (int x = 0; x < GetScreenWidth(); x += bgSpan)
      DrawTextureRec(backgroundTexture,
                     (Rectangle){xBgOffset, yBgOffset, bgSpan, bgSpan},
                     (Vector2){x, y}, WHITE);

  drawShipStats(&enemy);
  drawShipStats(&player);

  enemy.alive ? drawShip(&enemy) : drawBlast(&enemy);
  player.alive ? drawShip(&player) : drawBlast(&player);

  for (int i = 0; i < maxShots; i++)
    drawShot(&enemy.shots[i]), drawShot(&player.shots[i]);

#ifdef DEBUG
  // center divider
  DrawLine(0, arenaHeight(), arenaWidth(), arenaHeight(), BLUE);
  drawShotsSpeednPositions(2, enemy, player);
  drawShipnShotsBounds(2, enemy, player);
  drawShipSpeednHealth(2, enemy, player);
  drawShipPosition(2, enemy, player);
#endif
}
