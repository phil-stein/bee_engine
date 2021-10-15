#pragma once
#ifndef RENDERER_H
#define RENDERER_H

#include "CGLM/cglm.h"

#include "global.h"
#include "object_data.h"

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

#ifdef EDITOR_ACT
void render_scene_mouse_pick();
void render_scene_outline();
#endif
void render_scene_shadows();
void render_scene_normal();
void render_scene_skybox();
void render_scene_screen();
// renderes a single mesh, causing one draw-call
void draw_mesh(mesh* _mesh, material* mat, vec3 pos, vec3 rot, vec3 scale, bee_bool rotate_global, bee_bool is_gizmo, vec3 gizmo_col);
void set_shader_uniforms(material* mat);

// clears all entities, etc, completely wiping the current scene
void renderer_clear_scene();

#ifdef EDITOR_ACT
int read_mouse_position_mouse_pick_buffer_color();
#endif

int   get_camera_ent_id();
int** get_transparent_ids_ptr();
int*  get_transparent_ids_len();
int** get_dir_light_ids_ptr();
int*  get_dir_light_ids_len();
int** get_point_light_ids_ptr();
int*  get_point_light_ids_len();
int** get_spot_light_ids_ptr();
int*  get_spot_light_ids_len();

void set_camera_ent_id(int id);

// set play / pause used for editor
void set_gamestate(bee_bool play, bee_bool _hide_gizmos);
bee_bool get_gamestate();
void set_all_scripts(bee_bool act);

int* get_draw_calls_per_frame();
int* get_verts_per_frame();
int* get_tris_per_frame();
// void set_all_gizmo_meshes(bee_bool act);

void renderer_set(render_setting setting, bee_bool value);
bee_bool* renderer_get(render_setting setting);
f32* get_exposure();
u32 get_color_buffer();
void get_bg_color(vec3 col);
void set_bg_color(vec3 col);


// entity_properties get_entity_properties(int index);
renderer_properties get_renderer_properties();
#endif