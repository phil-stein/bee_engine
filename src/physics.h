#pragma once
#ifndef PHYSICS_H
#define PHYSICS_H

#include "CGLM\cglm.h"

#include "object_data.h"
#include "global.h"

void physics_init();
void physics_update();


// taken from "WinterDev: Designing a Physics Engine in 5 minutes", "https://www.youtube.com/watch?v=-_lspRG548E&t=301s"
void simulate_dynamics(entity* e);


collision_info collision_check_sphere_v_sphere(entity* s1, entity* s2);

// taken from "https://developer.mozilla.org/en-US/docs/Games/Techniques/3D_collision_detection"
collision_info collision_check_aabb_v_aabb(entity* b1, entity* b2);

collision_info collision_check_aabb_v_sphere(entity* b, entity* s);


void collision_response(entity* e1, entity* e2, collision_info info);
void collision_response_resolve_position(entity* e1, entity* e2, collision_info info);
void collision_response_resolve_velocity(entity* e1, entity* e2, collision_info info);

#endif