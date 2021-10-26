#pragma once
#ifndef PHYS_PHYSICS_H
#define PHYS_PHYSICS_H

#include "global.h"
#include "phys_types.h"
#include "types/object_data.h" 

// initialize physics engine
void physics_init();

// call once a frame to update the state of the physics engine
// "dt" pass delta time, the time passed since last frame
void physics_update(f32 dt);

#endif