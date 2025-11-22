#include "raylib.h"
#include "utils.hpp"
#include <algorithm>
#include <string>
#include <vector>

#define FLOOR 450.0f
#define CEILING 0.0f

#define PIPE_TIMER_MAX 150
#define PIPE_MOVE_SPEED 140
#define PIPE_START_X 1000
#define PIPE_GAP_MIN 110
#define PIPE_GAP_MAX 150
#define PIPE_GAP_Y_MIN 100
#define PIPE_GAP_Y_MAX FLOOR - PIPE_GAP_MAX - 25
#define PIPE_WIDTH 75

class Player {
public:
  Rectangle rect = {50, 200, 50, 50};
  Rectangle hitbox = {50, 50, 50, 20};
  Vector2 hitbox_offset = {00, 15};
  Vector2 vel = {0, 0};
  float gravity = 8.5;
  float jump_force = -300;
  Texture2D birb = LoadTexture("assets/bird.png");
  float angle = 0.0f;
  bool falling = false;

  void update_vel() {
    rect.x += vel.x;
    rect.y += vel.y;
  }

  void update_hitbox() {
    hitbox.x = rect.x + hitbox_offset.x;
    hitbox.y = rect.y + hitbox_offset.y;
  }

  void update(float dt) {
    if (falling)
      vel.y += gravity * dt;

    if (IsKeyPressed(KEY_SPACE)) {
      vel.y = jump_force * dt;
      falling = true;
    }

    update_vel();
    update_hitbox();

    if (hitbox.y + hitbox.height >= FLOOR) {
      hitbox.y = FLOOR - hitbox.height;
      rect.y = hitbox.y - hitbox_offset.y;
      vel.y = 0;
    }
    if (hitbox.y <= CEILING) {
      hitbox.y = CEILING;
      rect.y = hitbox.y - hitbox_offset.y;
      vel.y = std::max(0.0f, vel.y);
    }
  }
  void draw() {
    // DrawRectangleRec(hitbox, RED);
    DrawTexturePro(birb, {vel.y < 0 ? 0.0f : 32.0f, 0, 16, 16}, rect, {0, 0},
                   angle, WHITE);
  }
};

class Pipe {
public:
  Rectangle rect;

  Pipe(Rectangle rect) : rect(rect) {}

  void draw() { DrawRectangleRec(rect, GREEN); }

  void update(float dt) { rect.x -= PIPE_MOVE_SPEED * dt; }
};

enum GAME_STATE { GAME_STATE_MENU, GAME_STATE_DEAD, GAME_STATE_INGAME };

int main() {
  InitWindow(600, 450, "GAME NAME");

  SetTargetFPS(60);

  SetWindowState(FLAG_WINDOW_RESIZABLE);
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitAudioDevice();

  RenderTexture2D target = LoadRenderTexture(600, 450);

  bool should_exit = false;

  Player p;

  std::vector<Pipe> pipes;
  int pipeTimer = 60; // every sixty frames

  Texture2D background[4];
  background[0] = LoadTexture("assets/clouds/1.png");
  background[1] = LoadTexture("assets/clouds/2.png");
  background[2] = LoadTexture("assets/clouds/3.png");
  background[3] = LoadTexture("assets/clouds/4.png");

  Music music = LoadMusicStream("assets/music.mp3");
  bool music_enabled = true;
  PlayMusicStream(music);
  if (!music_enabled)
    PauseMusicStream(music);

  Sound death_sound = LoadSound("assets/death.wav");

  float background_scrolling[4] = {0, 0, 0, 0};

  int score = 0;

  GAME_STATE game_state = GAME_STATE_MENU;

  while (!WindowShouldClose() && !should_exit) {
    float dt = GetFrameTime();

    p.update(dt);

    switch (game_state) {
    case GAME_STATE_INGAME: {
      pipeTimer--;
      if (pipeTimer % (PIPE_TIMER_MAX / 5) == 0)
        score++;
      if (pipeTimer <= 0) {
        pipeTimer = PIPE_TIMER_MAX + GetRandomValue(-10, 10);
        int gap_size = GetRandomValue(PIPE_GAP_MIN, PIPE_GAP_MAX);
        int gap_y = GetRandomValue(PIPE_GAP_Y_MIN, PIPE_GAP_Y_MAX);
        // top pipe
        pipes.push_back(Pipe({PIPE_START_X, 0, PIPE_WIDTH, (float)gap_y}));
        // bottom pipe
        pipes.push_back(Pipe({PIPE_START_X, (float)gap_y + gap_size, PIPE_WIDTH,
                              (float)FLOOR - (gap_y)}));
      }

      for (Pipe &pipe : pipes) {
        pipe.update(dt);
        if (CheckCollisionRecs(pipe.rect, p.hitbox)) {
          game_state = GAME_STATE_DEAD;
          p.falling = false;
          p.vel = {0, 0};
          PlaySound(death_sound);
        }
      }

      pipes.erase(std::remove_if(pipes.begin(), pipes.end(),
                                 [](const Pipe &pipe) {
                                   return pipe.rect.x + pipe.rect.width < 0;
                                 }),
                  pipes.end());
    } break;
    case GAME_STATE_MENU: {
      if (IsKeyPressed(KEY_SPACE))
        game_state = GAME_STATE_INGAME;
      if (IsKeyPressed(KEY_M)) {
        music_enabled = !music_enabled;
        if (!music_enabled)
          PauseMusicStream(music);
        else
          ResumeMusicStream(music);
      }
    } break;
    case GAME_STATE_DEAD: {
      if (IsKeyPressed(KEY_SPACE)) {
        game_state = GAME_STATE_MENU;
        pipes.clear();
        score = 0;
        p = Player();
      }
    } break;
    }

    if (music_enabled)
      UpdateMusicStream(music);

    ClearBackground(WHITE);
    BeginTextureMode(target);
    ClearBackground(BLACK);

    for (int i = 0; i < 4; i++) {
      background_scrolling[i] -= i * dt * 50; // Changed += to -=
      if (background_scrolling[i] < 0) {
        background_scrolling[i] += 600.0f;
      }
      DrawTexturePro(background[i], {0, 0, 574, 326},
                     {background_scrolling[i], 0, 600, 450}, {0, 0}, 0.0f,
                     WHITE);
      DrawTexturePro(background[i], {0, 0, 574, 326},
                     {background_scrolling[i] - 600, 0, 600, 450}, {0, 0}, 0.0f,
                     WHITE);
    }
    for (Pipe &pipe : pipes) {
      pipe.draw();
    }
    p.draw();

    std::string score_str = "Score: " + std::to_string(score);
    DrawText(score_str.c_str(), 2, FLOOR - 26, 24, WHITE);

    if (game_state == GAME_STATE_MENU) {
      DrawText("FLAPPY BIRD", 600 / 2 - MeasureText("FLAPPY BIRD", 32) / 2, 32,
               32, WHITE);
      DrawText("PRESS SPACE TO START",
               600 / 2 - MeasureText("PRESS SPACE TO START", 24) / 2, 74, 24,
               WHITE);
      DrawText("press m to toggle music",
               600 / 2 - MeasureText("press m to toggle music", 24) / 2, 104,
               24, WHITE);
    }
    if (game_state == GAME_STATE_DEAD) {
      DrawText("You Died!", 600 / 2 - MeasureText("You Died!", 32) / 2, 32, 32,
               WHITE);
      DrawText(score_str.c_str(),
               600 / 2 - MeasureText(score_str.c_str(), 24) / 2, 74, 24, WHITE);
      DrawText("press space to continue",
               600 / 2 - MeasureText("press space to continue", 24) / 2, 104,
               24, WHITE);
    }
    EndTextureMode();

    BeginDrawing();
    DrawRenderTexture(target, 320, 240);
    DrawFPS(0, 0);
    EndDrawing();
  }

  UnloadRenderTexture(target);

  CloseAudioDevice();

  CloseWindow();
  return 0;
}
