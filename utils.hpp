#pragma once
#include "raylib.h"
#include <cmath>

inline void DrawRenderTexture(RenderTexture2D target, int targetWidth,
                              int targetHeight) {
  int screenWidth = GetScreenWidth();
  int screenHeight = GetScreenHeight();
  float scale = fminf((float)screenWidth / targetWidth,
                      (float)screenHeight / targetHeight);
  int renderWidth = (int)(targetWidth * scale);
  int renderHeight = (int)(targetHeight * scale);
  int offsetX = (screenWidth - renderWidth) / 2;
  int offsetY = (screenHeight - renderHeight) / 2;

  DrawTexturePro(
      target.texture,
      {0, 0, (float)target.texture.width, -(float)target.texture.height},
      {(float)offsetX, (float)offsetY, (float)renderWidth, (float)renderHeight},
      {0, 0}, 0, WHITE);
}

inline Vector2 GetScreenMousePos(Camera2D cam) {
  float widthRatio = (float)GetScreenWidth() / 320;
  float heightRatio = (float)GetScreenHeight() / 240;
  float scale = (widthRatio < heightRatio) ? widthRatio : heightRatio;
  int renderWidth = (int)(320 * scale);
  int renderHeight = (int)(240 * scale);
  int offsetX = (GetScreenWidth() - renderWidth) / 2;
  int offsetY = (GetScreenHeight() - renderHeight) / 2;
  Vector2 mouseScreenPos = GetMousePosition();

  Vector2 renderMouse = {(mouseScreenPos.x - offsetX) / scale,
                         (mouseScreenPos.y - offsetY) / scale};

  Vector2 mousePos = GetScreenToWorld2D(renderMouse, cam);

  return mousePos;
};
