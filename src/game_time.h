#pragma once
#ifndef GAME_TIME_H
#define GAME_TIME_H

#include "global.h"

// get the time passed since the last frame
f32 get_delta_time();
// !!! only used internally !!!
void set_delta_time(f32 delta_t);

// get the total time passed since the application was started
f32 get_total_secs();
// !!! only used internally !!!
void set_total_secs(f32 secs);

// get the current amount of frames processed per second
f32 get_fps();
// !!! only used internally !!!
void set_fps(f32 fps);

#endif

