#include "scene.h"

#include "timer.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

void SceneManagerInit(SceneManager* m) {
  m->transition.startedAt = -1;
  m->transition.endedAt   = 0;
  m->transition.ctx       = NULL;
  m->transition.cb        = NULL;
  m->transition.timer     = TimerCreate((TimerConfig){
      .direction  = TimerDirection_Forward,
      .iterations = 1,
  });

  m->indexes.curr = 0;
  m->indexes.next = -1;
  m->indexes.prev = -1;

  m->frame = 0;
}

Scene SceneCreate(
    int width,
    int height,
    SceneCallback enter,
    SceneCallback exit,
    SceneCallback draw,
    SceneCallback update) {
  return (Scene){
      .active    = false,
      .entering  = false,
      .leaving   = false,
      .enteredAt = -1,
      .leftAt    = -1,
      .canvas    = LoadRenderTexture(width, height),
      .src       = (Rectangle){0, 0, width, -height},
      .dst       = (Rectangle){0, 0, width, height},
      .origin    = (Vector2){0, 0},
      .rotation  = 0,
      .tint      = WHITE,
      .enter     = enter,
      .exit      = exit,
      .draw      = draw,
      .update    = update,
  };
}

void SceneDestroy(Scene scene) {
  UnloadRenderTexture(scene.canvas);
}

void SceneAppend(SceneManager* m, Scene scene) {
  Scene* entries = m->scenes.entries;
  int* size      = &m->scenes.size;
  if (*size == 0) scene.active = true;
  entries[*size] = scene;
  *size += 1;
}

float SceneGetTransitionProgress(SceneManager* m) {
  return TimerGetProgress(&m->transition.timer);
}

bool SceneTransitionTo(SceneManager* m, int next, int duration, SceneTransitionCallback cb, void* ctx) {
  if (m->indexes.next != -1) return true;
  m->scenes.entries[next].active      = true;
  m->scenes.entries[next].entering    = true;
  m->indexes.next                     = next;
  m->transition.startedAt             = m->frame + 1;
  m->transition.cb                    = cb;
  m->transition.ctx                   = ctx;
  m->transition.timer.config.duration = duration;
  TimerReset(&m->transition.timer);
  return false;
}

void SceneDraw(SceneManager* m, RenderTexture2D target, void* ctx) {
  Scene* entries = m->scenes.entries;
  int size       = m->scenes.size;
  for (int i = 0; i < size; i++) {
    Scene* s = &entries[i];
    if (s->active) {
      BeginTextureMode(s->canvas);
      {
        if (s->draw) s->draw(ctx);
      }
      EndTextureMode();
      BeginTextureMode(target);
      {
        DrawTexturePro(
            s->canvas.texture,
            s->src,
            s->dst,
            s->origin,
            s->rotation,
            s->tint);
      }
      EndTextureMode();
    }
  }
}

void SceneUpdate(SceneManager* m, void* ctx) {
  Scene* entries = m->scenes.entries;
  if (m->frame == 0) {
    Scene* curr = &entries[m->indexes.curr];
    if (curr->enter) {
      curr->enter(ctx);
    }
  }
  //* Update active scenes
  int size = m->scenes.size;
  for (int i = 0; i < size; i++) {
    Scene* s = &entries[i];
    if (s->active && s->update) {
      s->update(ctx);
    }
  }
  //* Update transition timer
  Timer* t = &m->transition.timer;
  if (m->indexes.next != -1) {
    //* Run transition callback
    SceneTransitionCallback cb = m->transition.cb;
    if (cb) {
      cb(m->indexes.curr, m->indexes.next, m->transition.ctx);
    }
    Scene* from    = &entries[m->indexes.curr];
    Scene* to      = &entries[m->indexes.next];
    float progress = TimerGetProgress(t);
    if (progress == 1) {
      //* Transition complete
      from->active          = false;
      from->entering        = false;
      m->indexes.prev       = m->indexes.curr;
      m->indexes.curr       = m->indexes.next;
      m->indexes.next       = -1;
      m->transition.endedAt = m->frame;
      TimerReset(t);
    } else {
      if (progress == 0) {
        if (from->exit) from->exit(ctx);
        if (to->enter) to->enter(ctx);
      }
      //* Transition in-progress
      TimerTick(t);
    }
  }
  //* Increment frame
  m->frame++;
}
