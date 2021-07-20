#pragma once
#ifndef RENDERER_H
#define RENDERER_H

#include "CGLM/cglm.h"

#include "global.h"
#include "object_data.h"
#include "ui.h"

typedef struct
{
	f32* perspective;
	f32* near_plane;
	f32* far_plane;

	f32* wireframe_col_r;
	f32* wireframe_col_g;
	f32* wireframe_col_b;
}renderer_properties;


void renderer_init();

void renderer_update();

void renderer_cleanup();


void draw_mesh(mesh* _mesh, material* mat, vec3 pos, vec3 rot, vec3 scale, enum bee_bool rotate_global);

// return the index to the entity
int add_entity(vec3 pos, vec3 rot, vec3 scale, mesh* _mesh, material* _material, light* _light, char* name);
void entity_switch_light_type(int entity_id, light_type new_type);
void entity_add_script(int entity_index, const char* name);
void entity_remove_script(int entity_index, int script_index);
void set_all_scripts(bee_bool act);
void set_all_light_meshes(bee_bool act);

void entity_remove(int entity_idx);

void add_entity_cube();

entity* get_entity_ptr(int idx);
// entity_properties get_entity_properties(int index);

renderer_properties get_renderer_properties();

void get_entity_len(int* _entities_len);

entity* get_entites();
entity* get_entity(int i);
int get_entity_id_by_name(char* name);


void renderer_enable_wireframe_mode(bee_bool act);
void renderer_enable_normal_mode(bee_bool act);
void renderer_enable_uv_mode(bee_bool act);

void renderer_set_skybox_active(bee_bool act);

#endif