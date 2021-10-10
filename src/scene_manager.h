#pragma once
#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "global.h"
#include "object_data.h"

char* get_active_scene_name();

void load_scene(char* name);
void save_scene(char* name);

#ifdef EDITOR_ACT
void save_scene_state();
void load_scene_state();
#endif

#endif
