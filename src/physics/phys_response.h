#pragma once
#ifndef PHYS_RESPONSE_H
#define PHYS_RESPONSE_H

#include "physics.h" // has phys_types.h, global.h, object_data.h

void collision_response(entity* e1, entity* e2, collision_info info);
void collision_response_resolve_position(entity* e1, entity* e2, collision_info info);
void collision_response_resolve_velocity(entity* e1, entity* e2, collision_info info);

#endif