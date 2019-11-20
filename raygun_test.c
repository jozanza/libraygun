#include "raygun.h"
#include <raylib.h>
#include <stdio.h>

// The Raygun singleton
// --------------------
Raygun* raygun = &RAYGUN_API;

// User-defined game context
// -------------------------
typedef struct GameContext {
  const char* name;
  int counter;
  RenderTexture2D offCanvas;
} GameContext;

// Lifecycle methods
// -----------------
// - init
// - destroy
// - draw
// - update

// Initialize
void init(void* ctx) {
  GameContext* game = ctx;
  printf("Initialized %s\n", game->name);
}

// Deinitialize
void destroy(void* ctx) {
  GameContext* game = ctx;
  printf("Destroyed %s\n", game->name);
}

// Draw each frame
void draw(void* ctx) {
  GameContext* game = ctx;
  // First, draw onto the game canvas
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
  // Then, draw the letterboxed game canvas into the window
  BeginDrawing();
  {
    raygun->clear();
    raygun->drawTexture(canvas.texture);
    DrawText("(^_^)", 0, 0, 24, WHITE);
  }
  EndDrawing();
}

// Update the context each frame
void update(void* ctx) {
  GameContext* game = ctx;
  game->counter++;
}

// Setup and launch the game
// -------------------------
int main(void) {
  // Bootstrap the lifecycle method context
  GameContext context = {
      .name = "My Game",
  };
  // Configure the game
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
