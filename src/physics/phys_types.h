#pragma once
#ifndef PHYS_TYPES_H
#define PHYS_TYPES_H

#include "global.h"
#include "CGLM/cglm.h"

typedef struct collision_info
{
	bee_bool collision;
	vec3 direction; // normal * depth of collision
	int entity_id;
	bee_bool trigger;

}collision_info;

#endif
