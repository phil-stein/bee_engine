#pragma once
#ifndef ENTITIES_H
#define ENTITIES_H

#include "CGLM/cglm.h"

#include "object_data.h"
#include "global.h"


void entities_init();
void entities_update();
void entities_cleanup();

void entities_clear_scene();

// only use this when certain id isnt taken, as it doesnt get checked properly.
int add_entity_direct_id(entity e, int id);
// add previously made entity to the engine
int add_entity_direct(entity e);
// makes entity from given properties and adds it to the engine
// if your goal is to simply add an entity to the game use this function
int add_entity(vec3 pos, vec3 rot, vec3 scale, mesh* _mesh, material* _material, camera* _cam, light* _light, char* name);
int duplicate_entity(int id);

// the transform of the entity with the parents transforms, calculated recursively 
void get_entity_global_transform(int idx, vec3* pos, vec3* rot, vec3* scale);

void entity_switch_light_type(int entity_id, light_type new_type);
void entity_add_script(int entity_index, const char* name);
void entity_remove_script(int entity_index, int script_index);

void entity_set_rot(int entity_id, vec3 rot);
// @TODO: not completely working
void entity_set_dir_vec(int entity_id, vec3 dir);


void entity_set_parent(int child, int parent);
void entity_remove_child(int parent, int child);
void entity_remove(int entity_idx);
void entity_change_id_idx(int entity_id, int new_id_idx);

void add_entity_cube();


// amount of entities at the moment
void get_entity_len(int* _entities_len);

// pointer to all entity ids
int* get_entity_ids(int* len);
// get an entity by its id
entity* get_entity(int id);
entity* get_cam_entity();
int get_cam_entity_id();
// returns the index to the entity
int get_entity_id_by_name(char* name);


#endif
