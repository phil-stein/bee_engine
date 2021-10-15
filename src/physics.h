#pragma once
#ifndef PHYSICS_H
#define PHYSICS_H

#include "CGLM\cglm.h"

#include "object_data.h"
#include "global.h"

void physics_init();
void physics_update();

void simulate_dynamics(entity* e);

bee_bool collision_check_sphere_v_sphere(entity* s1, entity* s2);


#endif