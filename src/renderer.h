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

typedef enum render_setting
{
	RENDER_WIREFRAME,
	RENDER_UV,
	RENDER_NORMAL,
	RENDER_CUBEMAP,
	RENDER_MSAA
}render_setting;


void renderer_init();

void renderer_update();

void renderer_cleanup();

// renderes a single mesh, amounting in one draw-call
void draw_mesh(mesh* _mesh, material* mat, vec3 pos, vec3 rot, vec3 scale, enum bee_bool rotate_global);

void renderer_clear_scene();

// the transform of the entity with the parents transforms, calculated recursively 
void get_entity_global_transform(int idx, vec3* pos, vec3* rot, vec3* scale);

int add_entity_direct(entity e);
int add_entity(vec3 pos, vec3 rot, vec3 scale, mesh* _mesh, material* _material, camera* _cam, light* _light, char* name);
int duplicate_entity(int id);

void entity_switch_light_type(int entity_id, light_type new_type);
void entity_add_script(int entity_index, const char* name);
void entity_remove_script(int entity_index, int script_index);
// set play / pause used for editor
void set_gamestate(bee_bool play, bee_bool _hide_gizmos);
bee_bool get_gamestate();
void set_all_scripts(bee_bool act);
// void set_all_gizmo_meshes(bee_bool act);

void renderer_set(render_setting setting, bee_bool value);
bee_bool* renderer_get(render_setting setting);

void entity_set_parent(int child, int parent);
void entity_remove_child(int parent, int child);
void entity_remove(int entity_idx);

void add_entity_cube();


// amount of entities at the moment
void get_entity_len(int* _entities_len);

// pointer to all entity ids
int* get_entity_ids(int* len);
// get an entity by its id
entity* get_entity(int id);
entity* get_cam_entity();
// returns the index to the entity
int get_entity_id_by_name(char* name);


// entity_properties get_entity_properties(int index);
renderer_properties get_renderer_properties();
#endif