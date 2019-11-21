#pragma once

#include "timer.h"
#include <raylib.h>

// clang-format off
#ifndef MAX_SCENE_COUNT
  #define MAX_SCENE_COUNT 1024
#endif
// clang-format on

typedef void (*SceneCallback)(void* ctx);

typedef struct Scene {
  bool active;
  bool entering;
  bool leaving;
  int enteredAt;
  int leftAt;
  RenderTexture2D canvas;
  Rectangle src;
  Rectangle dst;
  Vector2 origin;
  float rotation;
  Color tint;
  SceneCallback enter;
  SceneCallback exit;
  SceneCallback draw;
  SceneCallback update;
} Scene;

typedef void (*SceneTransitionCallback)(int from, int to, void* ctx);

typedef struct SceneManager {
  struct {
    Scene entries[MAX_SCENE_COUNT];
    int size;
  } scenes;
  struct {
    int startedAt;
    int endedAt;
    Timer timer;
    void* ctx;
    SceneTransitionCallback cb;
  } transition;
  struct {
    int curr;
    int next;
    int prev;
  } indexes;
  int frame;
} SceneManager;

//* Creates a Scene
Scene SceneCreate(
    int width,
    int height,
    SceneCallback enter,
    SceneCallback exit,
    SceneCallback draw,
    SceneCallback update);

//* Destroys a Scene
void SceneDestroy(Scene scene);

//* Initalizes the SceneManager
void SceneManagerInit(SceneManager* m);

//* Appends a Scene to the SceneManager
void SceneAppend(SceneManager* m, Scene scene);

//* Gets the current transition progress
float SceneGetTransitionProgress(SceneManager* m);

//* Draws the current scene
void SceneDraw(SceneManager* m, RenderTexture2D target, void* ctx);

//* Updates the current scene and performs internal updates
void SceneUpdate(SceneManager* m, void* ctx);

//* Initiates a transition from the current scene to another
bool SceneTransitionTo(SceneManager* m, int n, int duration, SceneTransitionCallback cb, void* ctx);
