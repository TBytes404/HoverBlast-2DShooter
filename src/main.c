// #define PLATFORM_WEB

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#include "game.h"

int main(void) {
  loadAssets();
  resetGame();
  PlayMusicStream(backgroundMusic);

#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(Game, 0, 1);
#else
  SetTargetFPS(frameRate);
  while (!WindowShouldClose())
    Game();
#endif

  unloadAssets();
  return 0;
}
