// #define PLATFORM_WEB

#if defined(PLATFORM_WEB)
#include <emscripten.h>
#endif

#include "game.h"

int main(void) {
  loadAssets();

  resetGame();
  PlayMusicStream(backgroundMusic);

#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(Play, 0, 1);
#else
  SetTargetFPS(frameRate);
  while (!WindowShouldClose())
    Play();
#endif

  unloadAssets();
  return 0;
}
