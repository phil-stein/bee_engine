#pragma once
#ifndef GAME_TIME_H
#define GAME_TIME_H

#include "global.h"

// calculates the new values for "get_delta_time()" & "get_fps()"
void game_time_update();

// get the time passed since the last frame
f32 get_delta_time();

// get the total time passed since the application was started
f32 get_total_secs();

// get the current amount of frames processed per second
f32 get_fps();

#endif

