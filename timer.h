#pragma once

#include <stdbool.h>

/**
 * Enumeration of Timer directions
 */
typedef enum TimerDirection {
  TimerDirection_Forward = 1,
  TimerDirection_Reverse = -1,
} TimerDirection;

/**
 * Mutable values held inside a timer
 */
typedef struct TimerValues {
  int delay;
  int duration;
  int iterations;
  float progress;
  TimerDirection direction;
} TimerValues;

/**
 * Immutable configuration for initializing a Timer
 */
typedef struct TimerConfig {
  TimerDirection direction;
  int delay;
  int duration;
  int iterations;
  bool pingpong;
} TimerConfig;

/**
 * A Timer
 */
typedef struct Timer {
  TimerValues values;
  TimerConfig config;
} Timer;

/**
 * Creates a new Timer
 */
Timer TimerCreate(TimerConfig config);

/**
 * Resets a Timer to its initial state
 * Returns -1 if failed
 */
int TimerReset(Timer* t);

/**
 * Ticks the timer
 * Returns the current progress
 */
float TimerTick(Timer* t);

/**
 * Gets the current progress of the Timer
 */
float TimerGetProgress(Timer* t);
