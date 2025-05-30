#pragma once
#include "utils.h"

typedef struct
{
    Vector2 pos;
    float radius, colider, speed, accel;
    bool active, fromEnemy;
} Shot;

void initShot(Shot *shot, float xPos, float yPos, float accel)
{
    shot->pos.x = xPos;
    shot->pos.y = yPos;
    shot->radius = shotsTexture.height / 2;
    shot->colider = shotsTexture.height / 3;
    shot->speed = 0;
    shot->accel = accel;
    shot->active = false;
    shot->fromEnemy = false;
}

void drawShot(Shot *shot)
{
    if (shot->active)
        DrawTexturePro(shotsTexture, (Rectangle){shot->fromEnemy ? 0 : shotsTexture.width / 2, 0, shotsTexture.width / 2, shotsTexture.height},
                       (Rectangle){shot->pos.x + shot->radius * (shot->fromEnemy ? 1 : -1), shot->pos.y, shotsTexture.width / 2, shotsTexture.height},
                       (Vector2){0, 0}, shot->fromEnemy ? 90 : -90, WHITE);
}

void moveShot(Shot *shot)
{
    if (shot->active)
    {
        shot->speed += shot->accel;
        if (shot->speed > maxShotSpeed)
            shot->speed = maxShotSpeed;

        shot->pos.y += shot->speed * GetFrameTime() * (shot->fromEnemy ? 1 : -1);
        if ((shot->fromEnemy ? shot->pos.y > GetScreenHeight() : shot->pos.y < 0))
            shot->active = false, shot->speed = 0;
    }
}

bool checkShot(Shot *shot, Rectangle rect)
{
    bool ret = false;
    if (shot->active &&
        CheckCollisionCircleRec((Vector2){shot->pos.x, shot->pos.y}, shot->colider, rect))
        shot->active = false, shot->speed = 0, ret = true;
    return ret;
}
