#pragma once
#ifndef GAME_TIME_H
#define GAME_TIME_H

#include "global.h"

f32 get_delta_time();
void set_delta_time(f32 delta_t);

f32 get_total_secs();
void set_total_secs(f32 secs);

f32 get_fps();
void set_fps(f32 fps);

#endif

