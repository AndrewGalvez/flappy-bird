#include "raylib.h"
#include "utils.hpp"

int main() {
  InitWindow(600, 450, "GAME NAME");

  SetTargetFPS(60);

  SetWindowState(FLAG_WINDOW_RESIZABLE);
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitAudioDevice();

  RenderTexture2D target = LoadRenderTexture(600, 450);

  bool should_exit = false;

  while (!WindowShouldClose() && !should_exit) {
    ClearBackground(WHITE);
    BeginTextureMode(target);
    ClearBackground(BLACK);

    EndTextureMode();

    BeginDrawing();
    DrawRenderTexture(target, 320, 240);
    DrawFPS(0, 0);
    EndDrawing();
  }

  UnloadRenderTexture(target);

  CloseWindow();
  return 0;
}
