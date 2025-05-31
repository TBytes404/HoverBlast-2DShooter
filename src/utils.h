#pragma once
#include <raylib.h>
// #define DEBUG

// Consts
const char windowTitle[] = "Hover Blast";
const int windowWidth = 900, windowHeight = 800;
const int fontSize = 24, bgSpan = 300, frameRate = 60;
const int maxShipSpeed = 500, maxShotSpeed = 600, maxShots = 12;
const int maxInputChars = 9, botMaxFrameDelay = 6, botMoveDirecProba = 100;

int arenaWidth(void) { return GetScreenWidth(); }
int arenaHeight(void) { return GetScreenHeight() / 2; }

// Assets
Music backgroundMusic;
Texture2D shotsTexture, shipTexture, shipBlastTexture, backgroundTexture;
Sound shootSound, blastSound, getShotSound;

void loadAssets(void) {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(windowWidth, windowHeight, windowTitle);
  InitAudioDevice();

  shipTexture = LoadTexture("resources/Ship.png");
  shotsTexture = LoadTexture("resources/Shots.png");
  shipBlastTexture = LoadTexture("resources/ShipBlast.png");
  backgroundTexture = LoadTexture("resources/Background.png");

  shootSound = LoadSound("resources/Shoot.wav");
  blastSound = LoadSound("resources/ShipBlast.wav");
  getShotSound = LoadSound("resources/GetShot.mp3");

  backgroundMusic = LoadMusicStream("resources/BackgroundMusic.mp3");
  backgroundMusic.looping = true;
}

void unloadAssets(void) {
  UnloadMusicStream(backgroundMusic);

  UnloadSound(getShotSound);
  UnloadSound(shootSound);
  UnloadSound(blastSound);

  UnloadTexture(backgroundTexture);
  UnloadTexture(shipBlastTexture);
  UnloadTexture(shotsTexture);
  UnloadTexture(shipTexture);

  CloseAudioDevice();
  CloseWindow();
}

// Bot
int botFrameDelay;
void initBot(void) { botFrameDelay = 0; }
int botFollowThresh(void) { return IsWindowFullscreen() ? 450 : 250; }

// Name Input
Rectangle inputTextRect;
int inputLetterCount, cursorBlinkDelay;
char nameInput[maxInputChars + 1] = "\0"; // Name is not reset on replay

void initTextInput(void) {
  inputTextRect = (Rectangle){0, 0, 0, 0};
  inputLetterCount = 0, cursorBlinkDelay = 0;
}

void updateTextInput(void) {
  cursorBlinkDelay++;
  SetMouseCursor(MOUSE_CURSOR_IBEAM);
  int key = GetCharPressed();
  while (key >
         0) // check if more characters have been pressed on the same frame
  {
    if ((key >= 33) && (key <= 125) && (inputLetterCount < maxInputChars)) {
      nameInput[inputLetterCount] = (char)key;
      nameInput[inputLetterCount + 1] =
          '\0'; // add null terminator at the end of the string
      inputLetterCount++;
    }
    key = GetCharPressed();
  }
  if (IsKeyPressed(KEY_BACKSPACE)) {
    inputLetterCount--;
    if (inputLetterCount < 0)
      inputLetterCount = 0;
    nameInput[inputLetterCount] = '\0';
  }
}

void drawTextInput(void) {
  DrawRectangleRec(inputTextRect, LIGHTGRAY);
  DrawRectangleLines(inputTextRect.x, inputTextRect.y, inputTextRect.width,
                     inputTextRect.height, RED);

  DrawText(nameInput, inputTextRect.x + 5, inputTextRect.y + 8, fontSize,
           MAROON);
  DrawText(TextFormat("INPUT CHARS: %i/%i", inputLetterCount, maxInputChars),
           inputTextRect.x, inputTextRect.y + inputTextRect.height + 8,
           fontSize / 1.5, DARKGRAY);
  // draw blinking underscore char
  if (inputLetterCount < maxInputChars)
    if (((cursorBlinkDelay / 20) % 2) == 0)
      DrawText("_", inputTextRect.x + 8 + MeasureText(nameInput, fontSize),
               inputTextRect.y + 12, fontSize, MAROON);
}

// Background Texture
int xBgOffset = bgSpan, yBgOffset = bgSpan;

void changeBackground(void) {
  if (xBgOffset == 0 && yBgOffset == 0)
    xBgOffset = bgSpan;
  else if (xBgOffset == bgSpan && yBgOffset == 0)
    yBgOffset = bgSpan;
  else if (xBgOffset == bgSpan && yBgOffset == bgSpan)
    xBgOffset = 0, yBgOffset = bgSpan;
  else if (xBgOffset == 0 && yBgOffset == bgSpan)
    xBgOffset = 0, yBgOffset = 0;
}

// Player Stats
void drawStats(char *name, int score, int health, int yPos, int fontSize,
               Color color) {
  DrawText(name, fontSize, yPos, fontSize, color);
  DrawText(TextFormat("Score %d", score), fontSize, yPos + fontSize, fontSize,
           color);
  DrawText(TextFormat("Health %d", health), fontSize, yPos + fontSize * 2,
           fontSize, color);
}

// Drawing Helper
void helpDraw(void (*draw)(void)) {
  BeginDrawing();
  ClearBackground(RAYWHITE);
  draw();
  EndDrawing();
}
