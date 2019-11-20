#include "raygun.h"

#include "scene.h"
#include "timer.h"
#include <raylib.h>
#include <stdlib.h>

typedef struct Canvas {
  RenderTexture2D target;
  int width;
  int height;
} Canvas;

typedef struct Window {
  const char* title;
  Color clearColor;
  bool allowResize;
  int fps;
} Window;

typedef struct GameContext {
  Canvas canvas;
  Window window;
  SceneManager sceneManager;
  RaygunConfig config;
  int frame;
} GameContext;

static GameContext game;

static void openWindow(int width, int height, int x, int y) {
  SetExitKey(-1); // disable close via ESC
  if (game.window.allowResize) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  }
  InitWindow(width, height, game.window.title);
  SetWindowMinSize(game.canvas.width, game.canvas.height);
  SetTargetFPS(game.window.fps);
  SetWindowPosition(x, y);
  game.canvas.target = LoadRenderTexture(game.canvas.width, game.canvas.height);
}

static void handleWindowResize() {
  if (IsWindowResized()) {
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    // TODO: find way to NOT reset window position on resize
    int x = 0;
    int y = 0;
    CloseWindow();
    UnloadRenderTexture(game.canvas.target);
    openWindow(w, h, x, y);
  }
}

static Rectangle calcLetterBox() {
  float windowW = GetScreenWidth();
  float windowH = GetScreenHeight();
  float canvasW = game.canvas.width;
  float canvasH = game.canvas.height;
  float scaleX  = windowW / canvasW;
  float scaleY  = windowH / canvasH;
  float scale   = scaleX < scaleY ? scaleX : scaleY;
  float w       = scale * canvasW;
  float h       = scale * canvasH;
  float x       = (windowW - w) * .5;
  float y       = (windowH - h) * .5;
  return (Rectangle){x, y, w, h};
}

/** Public Methods **/

static void API_start(RaygunConfig config) {
  //* Initialize
  game.config             = config;
  game.window.title       = config.windowTitle == NULL ? "" : config.windowTitle;
  game.window.clearColor  = config.windowColor;
  game.window.allowResize = config.windowResize;
  game.window.fps         = config.fps;
  game.canvas.width       = config.width;
  game.canvas.height      = config.height;
  SceneManagerInit(&game.sceneManager);
  SetTraceLogLevel(config.logLevel);
  openWindow(config.windowWidth, config.windowHeight, 0, 0);
  if (config.init) config.init(config.context);
  //* Draw + Update
  while (!WindowShouldClose()) {
    handleWindowResize();
    if (config.draw) config.draw(config.context);
    if (config.update) config.update(config.context);
    game.frame++;
  }
  //* Destroy
  if (config.destroy) config.destroy(config.context);
  UnloadRenderTexture(game.canvas.target);
  CloseWindow();
}

static int API_width(void) {
  return game.canvas.width;
}

static int API_height(void) {
  return game.canvas.height;
}

static void API_clear(void) {
  ClearBackground(game.window.clearColor);
}

static RenderTexture2D API_canvas(void) {
  return game.canvas.target;
}

static void API_drawTexture(Texture2D texture) {
  Rectangle src  = {0, 0, texture.width, -texture.height};
  Rectangle dst  = calcLetterBox();
  Vector2 origin = {0, 0};
  float rotation = 0;
  Color tint     = WHITE;
  DrawTexturePro(
      texture,
      src,
      dst,
      origin,
      rotation,
      tint);
}

static void API_addScene(RaygunScene* scene) {
  //* Create the scene
  Scene s = SceneCreate(
      game.canvas.width,
      game.canvas.height,
      scene->enter,
      scene->exit,
      scene->draw,
      scene->update);
  //* Assign the scene ID
  scene->id = game.sceneManager.scenes.size;
  //* Add to the manager
  SceneAppend(&game.sceneManager, s);
}

static void API_drawScenes(void) {
  SceneDraw(&game.sceneManager, game.canvas.target, game.config.context);
}

static void API_updateScenes(void) {
  SceneUpdate(&game.sceneManager, game.config.context);
}

Raygun RAYGUN_API = {
    .start        = API_start,
    .width        = API_width,
    .height       = API_height,
    .clear        = API_clear,
    .canvas       = API_canvas,
    .drawTexture  = API_drawTexture,
    .addScene     = API_addScene,
    .drawScenes   = API_drawScenes,
    .updateScenes = API_updateScenes,
    // .frame
    // .transitionTo
    // .transitionProgress
};
