#pragma once
#include "game.h"

#if defined(PLATFORM_WEB)
#include <emscripten/html5.h>
#endif

void updateMenu(void) {
  if ((IsKeyPressed(KEY_LEFT_CONTROL) || IsKeyPressed(KEY_RIGHT_CONTROL)) &&
      pause)
    pause = false;

  else if (IsKeyPressed(KEY_ENTER))
    resetGame();

  else if (IsKeyPressed(KEY_TAB))
    changeBackground();

  else if (IsKeyPressed(KEY_SPACE))
#if defined(PLATFORM_WEB)
    ToggleFullscreen();
#else
    IsWindowFullscreen()
        ? (ToggleFullscreen(), SetWindowSize(windowWidth, windowHeight))
        : (SetWindowSize(GetMonitorWidth(GetCurrentMonitor()),
                         GetMonitorHeight(GetCurrentMonitor())),
           ToggleFullscreen());
#endif

  updateTextInput();
}

void drawMenu(void) {
  const char gameTitleText[] = "Hover Blast";
  const char nameInputText[] = "Player Name: ";
  const char replayText[] = "Press [Enter] to Play/Replay";
  const char toggleFullscreenText[] = "Press [Space] to Toggle FullScreen";
  const char changeBackgroundText[] = "Press [Tab] to Change Background";
  const char togglePauseText[] = "Press [Ctrl] to Pause/Resume";
  const char *inGameKeysText =
      "[W A S D]/[UP LEFT DOWN RIGHT] to Move & [Space/F] to Shoot"; // [⬆ ⬅ ⬇
                                                                     // ⮕]

  drawGame();
  const int bgWidth = MeasureText(inGameKeysText, fontSize) + fontSize,
            bgHeight = fontSize * 7 * 3;
  DrawRectangle((GetScreenWidth() / 2 - bgWidth / 2),
                (GetScreenHeight() / 2 - bgHeight / 2), bgWidth, bgHeight,
                RAYWHITE);

  DrawText(gameTitleText,
           GetScreenWidth() / 2 - MeasureText(gameTitleText, 32) / 2,
           arenaHeight() - 32 * 7, 32, VIOLET);
  DrawText(nameInputText,
           GetScreenWidth() / 2 - MeasureText(nameInputText, fontSize),
           arenaHeight() - 32 * 4, fontSize, GRAY);
  inputTextRect = (Rectangle){(GetScreenWidth() / 2.) - (225 / 2.),
                              arenaHeight() - 32 * 3, 225, 32},
  drawTextInput();
  DrawText(replayText,
           GetScreenWidth() / 2 - MeasureText(replayText, fontSize) / 2,
           arenaHeight() - fontSize * 1, fontSize, LIME);
  DrawText(toggleFullscreenText,
           GetScreenWidth() / 2 -
               MeasureText(toggleFullscreenText, fontSize) / 2,
           arenaHeight() - fontSize * -1, fontSize, RED);
  DrawText(changeBackgroundText,
           GetScreenWidth() / 2 -
               MeasureText(changeBackgroundText, fontSize) / 2,
           arenaHeight() - fontSize * -5, fontSize, ORANGE);
  DrawText(togglePauseText,
           GetScreenWidth() / 2 - MeasureText(togglePauseText, fontSize) / 2,
           arenaHeight() - fontSize * -3, fontSize, BLUE);
  DrawText(inGameKeysText,
           GetScreenWidth() / 2 - MeasureText(inGameKeysText, fontSize) / 2,
           arenaHeight() - fontSize * -7, fontSize, DARKGRAY);
}

void Play(void) {
  gameover || pause ? (updateMenu(), helpDraw(drawMenu))
                    : (updateGame(), helpDraw(drawGame));
}
