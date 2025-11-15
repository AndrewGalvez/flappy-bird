#include "raylib.h"
#include "utils.hpp"

class Player {
public:
  Rectangle rect = {50, 50, 50, 50};
  Rectangle hitbox = {50, 50, 50, 20};
  Vector2 hitbox_offset = {00, 15};
  Vector2 vel = {0, 0};
  float gravity = 8.5;
  float jump_force = -300;
  Texture2D birb = LoadTexture("assets/bird.png");
  float angle = 0.0f;

  void update_vel() {
    rect.x += vel.x;
    rect.y += vel.y;
  }

  void update_hitbox() {
    hitbox.x = rect.x + hitbox_offset.x;
    hitbox.y = rect.y + hitbox_offset.y;
  }

  void update(float dt) {
    vel.y += gravity * dt;

    if (IsKeyPressed(KEY_SPACE)) {
      vel.y = jump_force * dt;
    }

    // bounds checking
    if (hitbox.y + hitbox.height + vel.y > 450 || hitbox.y + hitbox.y < 0)
      vel.y *= -0.8;

    update_vel();
    update_hitbox();
  }

  void draw() {
    // DrawRectangleRec(hitbox, RED);
    DrawTexturePro(birb, {vel.y < 0 ? 0.0f : 32.0f, 0, 16, 16}, rect, {0, 0},
                   angle, WHITE);
  }
};

int main() {
  InitWindow(600, 450, "GAME NAME");

  SetTargetFPS(60);

  SetWindowState(FLAG_WINDOW_RESIZABLE);
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitAudioDevice();

  RenderTexture2D target = LoadRenderTexture(600, 450);

  bool should_exit = false;

  Player p;

  Texture2D background[4];
  background[0] = LoadTexture("assets/clouds/1.png");
  background[1] = LoadTexture("assets/clouds/2.png");
  background[2] = LoadTexture("assets/clouds/3.png");
  background[3] = LoadTexture("assets/clouds/4.png");

  float background_scrolling[4] = {0, 0, 0, 0};

  while (!WindowShouldClose() && !should_exit) {
    float dt = GetFrameTime();

    p.update(dt);

    ClearBackground(WHITE);
    BeginTextureMode(target);
    ClearBackground(BLACK);

    for (int i = 0; i < 4; i++) {
      background_scrolling[i] += i * dt * 50;
      background_scrolling[i] = fmod(background_scrolling[i], 574.0f);
      DrawTexturePro(background[i], {background_scrolling[i], 0, 574, 326},
                     {0, 0, 600, 450}, {0, 0}, 0.0f, WHITE);
    }

    p.draw();

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
