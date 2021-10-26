#pragma once
#ifndef PHYS_DYNAMICS_H
#define PHYS_DYNAMICS_H

#include "physics.h" // has phys_types.h, global.h, object_data.h

// taken from "WinterDev: Designing a Physics Engine in 5 minutes", "https://www.youtube.com/watch?v=-_lspRG548E&t=301s"
void simulate_dynamics(entity* e, f32 dt);

#endif