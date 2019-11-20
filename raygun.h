#pragma once

#include <raylib.h>
#include <stdbool.h>

typedef struct RaygunConfig {
  TraceLogType logLevel;
  void* context;
  const char* windowTitle;
  Color windowColor;
  int windowWidth;
  int windowHeight;
  bool windowResize;
  int width;
  int height;
  int fps;
  void (*init)(void* ctx);
  void (*destroy)(void* ctx);
  void (*draw)(void* ctx);
  void (*update)(void* ctx);
} RaygunConfig;

typedef struct RaygunScene {
  int id;
  void (*enter)(void* ctx);
  void (*exit)(void* ctx);
  void (*draw)(void* ctx);
  void (*update)(void* ctx);
} RaygunScene;

typedef struct Raygun {
  void (*start)(RaygunConfig cfg);
  int (*width)(void);
  int (*height)(void);
  void (*clear)(void);
  RenderTexture2D (*canvas)(void);
  void (*drawTexture)(Texture2D texture);
  void (*addScene)(RaygunScene* scene);
  void (*drawScenes)(void);
  void (*updateScenes)(void);
} Raygun;

Raygun RAYGUN_API;
