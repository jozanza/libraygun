#include "raygun.h"
#include <raylib.h>
#include <stdio.h>

Raygun* raygun = &RAYGUN_API;

typedef struct State {
  const char* name;
  int counter;
  RenderTexture2D offCanvas;
} State;

void init(void* ctx) {
  State* game = ctx;
  printf("Initialized %s\n", game->name);
}

void destroy(void* ctx) {
  State* game = ctx;
  printf("Destroyed %s\n", game->name);
}

void draw(void* ctx) {
  State* game            = ctx;
  RenderTexture2D canvas = raygun->canvas();
  BeginTextureMode(canvas);
  {
    ClearBackground(RED);
    DrawText(
        TextFormat("size: %dx%d, frame: %d", raygun->width(), raygun->height(), game->counter),
        16,
        16,
        10,
        RAYWHITE);
  }
  EndTextureMode();
  BeginDrawing();
  {
    raygun->clear();
    raygun->drawTexture(canvas.texture);
    DrawText("(^_^)", 0, 0, 24, WHITE);
  }
  EndDrawing();
}

void update(void* ctx) {
  State* game = ctx;
  game->counter++;
}

int main(void) {
  State context = {
      .name = "Test Game",
  };
  RaygunConfig cfg = {
      .logLevel     = LOG_ERROR,
      .context      = &context,
      .windowTitle  = context.name,
      .windowWidth  = 800,
      .windowHeight = 600,
      .windowResize = true,
      .width        = 240,
      .height       = 320,
      .init         = init,
      .destroy      = destroy,
      .draw         = draw,
      .update       = update,
  };
  raygun->start(cfg);
}
