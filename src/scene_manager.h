#pragma once
#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "global.h"
#include "object_data.h"

void load_scene(char* name);
void save_scene(char* name);

char* get_active_scene_name();

#endif
