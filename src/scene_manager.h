#pragma once
#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "global.h"
#include "object_data.h"

char* get_scene_dir_path();
char* get_active_scene_name();

void load_scene(char* name);
void save_scene(char* name);

void clear_scene();

void add_empty_scene(const char* name);
void add_default_scene(const char* name);

#ifdef EDITOR_ACT
void save_scene_state();
void load_scene_state();
#endif

#endif
