#include "raygun.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

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

typedef enum TransitionDirection {
  TransitionDirection_Next = -1,
  TransitionDirection_Back = 1,
} TransitionDirection;

void swipe(int from, int to, void* ctx) {
  int dir = (int)ctx;
  float p = raygun->transitionProgress();
  // printf("progress: %.2f\n", p);
  int width   = raygun->width();
  int offsetX = p * (float)width;
  int fromX   = offsetX * dir;
  int toX     = fromX + (width * -dir);
  raygun->translateScene(from, fromX, 0);
  raygun->translateScene(to, toX, 0);
}

// Scenes
// ------
RaygunScene sceneA;
RaygunScene sceneB;
RaygunScene sceneC;

//Scene A
void enterSceneA(void* ctx) {
  GameContext* game = ctx;
  printf("Entered Scene A!\n");
}
void exitSceneA(void* ctx) {
  GameContext* game = ctx;
  printf("Exited Scene A!\n");
}
void drawSceneA(void* ctx) {
  GameContext* game = ctx;
  ClearBackground(RED);
  DrawText(
      TextFormat("SCENE A!!! size: %dx%d, frame: %d", raygun->width(), raygun->height(), game->counter),
      16,
      16,
      10,
      RAYWHITE);
}
void updateSceneA(void* ctx) {
  GameContext* game = ctx;
  game->counter++;
  if (IsKeyPressed(KEY_RIGHT)) {
    raygun->transitionTo(
        sceneB.id,
        raygun->seconds(1),
        swipe,
        (void*)TransitionDirection_Next);
  }
}
RaygunScene sceneA = {
    .enter  = enterSceneA,
    .exit   = exitSceneA,
    .draw   = drawSceneA,
    .update = updateSceneA,
};

//Scene B
void enterSceneB(void* ctx) {
  GameContext* game = ctx;
  printf("Entered Scene B!\n");
}
void exitSceneB(void* ctx) {
  GameContext* game = ctx;
  printf("Exited Scene B!\n");
}
void drawSceneB(void* ctx) {
  GameContext* game = ctx;
  ClearBackground(GREEN);
}
void updateSceneB(void* ctx) {
  GameContext* game = ctx;
  if (IsKeyPressed(KEY_LEFT)) {
    raygun->transitionTo(
        sceneA.id,
        raygun->seconds(.5),
        swipe,
        (void*)TransitionDirection_Back);
  }
  if (IsKeyPressed(KEY_RIGHT)) {
    raygun->transitionTo(
        sceneC.id,
        raygun->seconds(.5),
        swipe,
        (void*)TransitionDirection_Next);
  }
}
RaygunScene sceneB = {
    .enter  = enterSceneB,
    .exit   = exitSceneB,
    .draw   = drawSceneB,
    .update = updateSceneB,
};

// Scene C
void enterSceneC(void* ctx) {
  GameContext* game = ctx;
  printf("Entered Scene C!\n");
}
void exitSceneC(void* ctx) {
  GameContext* game = ctx;
  printf("Exited Scene C!\n");
}
void drawSceneC(void* ctx) {
  GameContext* game = ctx;
  ClearBackground(BLUE);
}
void updateSceneC(void* ctx) {
  GameContext* game = ctx;
  if (IsKeyPressed(KEY_LEFT)) {
    raygun->transitionTo(
        sceneB.id,
        raygun->seconds(.5),
        swipe,
        (void*)TransitionDirection_Back);
  }
}
RaygunScene sceneC = {
    .enter  = enterSceneC,
    .exit   = exitSceneC,
    .draw   = drawSceneC,
    .update = updateSceneC,
};

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
  raygun->addScene(&sceneA);
  raygun->addScene(&sceneB);
  raygun->addScene(&sceneC);
}

// Deinitialize
void destroy(void* ctx) {
  GameContext* game = ctx;
  printf("Destroyed %s\n", game->name);
}

// Draw each frame
void draw(void* _ctx) {
  raygun->drawScenes();
  BeginDrawing();
  {
    raygun->clear();
    raygun->drawTexture(raygun->canvas().texture);
    DrawText("(^_^)", 0, 0, 24, WHITE);
  }
  EndDrawing();
}

// Update the context each frame
void update(void* _ctx) {
  raygun->updateScenes();
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
