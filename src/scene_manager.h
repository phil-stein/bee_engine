#pragma once
#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "global.h"
#include "object_data.h"

// get the path to the current directory that stores the scene files
char* get_scene_dir_path();
// get the name of the currently loaded scene
// if no scene file is loaded returns "no_name"
char* get_active_scene_name();

// load a scene by the given name
void load_scene(char* name);
// save the currently loaded scene to a file with its current name
// overwrites previous file with its own name
void save_scene(char* name);

// clears all parts of the currently loaded scene
// resets the current scene name to "no_name"
void clear_scene();

// adds a completely empty scene file to the scene directory
// adds a basic scene file to the scene directory
void add_empty_scene(const char* name);
void add_default_scene(const char* name);

#ifdef EDITOR_ACT
// stores the current scenes state
// using "load_scene_state()" this state can be restored
void save_scene_state();
// restores the scene state saved using "save_scene_state()"
void load_scene_state();
#endif

#endif
