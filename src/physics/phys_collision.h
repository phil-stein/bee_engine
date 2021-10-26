#pragma once
#ifndef PHYS_COLLISION_H
#define PHYS_COLLISION_H

#include "physics.h" // has phys_types.h, global.h, object_data.h

// check collision between two entities
collision_info collision_check(entity* e1, entity* e2);

collision_info collision_check_sphere_v_sphere(entity* s1, entity* s2);

// taken from "https://developer.mozilla.org/en-US/docs/Games/Techniques/3D_collision_detection"
collision_info collision_check_aabb_v_aabb(entity* b1, entity* b2);

collision_info collision_check_aabb_v_sphere(entity* b, entity* s);

#endif
