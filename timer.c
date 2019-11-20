#include "timer.h"

#include <stdbool.h>

typedef struct TimerPrivate {
  int delay;
  int duration;
  int iterations;
  float progress;
  TimerDirection direction;
} TimerPrivate;

Timer TimerCreate(TimerConfig config) {
  Timer t = {
      .config = config,
  };
  TimerReset(&t);
  return t;
}

int TimerReset(Timer* t) {
  TimerValues* val = &t->values;
  TimerConfig* cfg = &t->config;
  val->direction   = cfg->direction;
  val->iterations  = cfg->iterations;
  val->delay       = cfg->delay;
  switch (val->direction) {
  case TimerDirection_Forward: {
    val->duration = 0;
    val->progress = 0;
  }; break;
  case TimerDirection_Reverse: {
    val->duration = cfg->duration;
    val->progress = 1;
  }; break;
  default:
    return -1;
  }
  return 0;
}

float TimerTick(Timer* t) {
  TimerValues* val = &t->values;
  TimerConfig* cfg = &t->config;
  if (val->iterations > 0) {
    if (val->delay > 0) val->delay--;
    if (!val->delay) {
      val->duration += cfg->direction;
      val->progress = (float)val->duration / (float)cfg->duration;
      bool starting = val->duration == 0;
      bool finished = val->duration == cfg->duration;
      if (finished || starting) {
        val->delay = cfg->delay;
        val->iterations -= 1;
        if (cfg->pingpong) {
          val->direction *= -1;
        }
      }
    }
  }
  return val->progress;
}

float TimerGetProgress(Timer* t) {
  return t->values.progress;
}