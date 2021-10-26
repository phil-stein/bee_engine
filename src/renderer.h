#pragma once
#ifndef RENDERER_H
#define RENDERER_H

#include "CGLM/cglm.h"

#include "global.h"
#include "types/object_data.h"

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

#ifdef EDITOR_ACT
typedef enum debug_draw_type
{
	DEBUG_DRAW_LINE,
	DEBUG_DRAW_SPHERE,
	DEBUG_DRAW_CUBE
}debug_draw_type;

typedef struct debug_draw
{
	debug_draw_type type;
	vec3 v1; // pos / pos1
	vec3 v2; // scale / pos2
}debug_draw;
#endif

void renderer_init();

void renderer_update();

void renderer_cleanup();

#ifdef EDITOR_ACT
// renders the scene with each object having a single color representing their id
void render_scene_mouse_pick();
// renders the currently selected entity as white
void render_scene_outline();
// renders different debug-primitives defines as "debug_draw_type" & "debug_draw"
// submit debug graphics via "debug_draw_sphere()", etc.
void render_scene_debug();
#endif
// renders the shadowmap for each light
void render_scene_shadows();
// render all the entities using lighting and hdr
void render_scene_normal();
// renders the cubemap into the background
void render_scene_skybox();
// renders the previous stages onto a quad spanning the whole window
void render_scene_screen();

// renderes a single mesh, causing one draw-call
void draw_mesh(int entity_id, mesh* _mesh, material* mat, vec3 pos, vec3 rot, vec3 scale, bee_bool rotate_global, bee_bool is_gizmo, vec3 gizmo_col);
// create model matrix from position, rotation and scale
// output passed to matrix "model", doesnt need to be identity
void make_model_matrix(int entity_id, vec3 pos, vec3 rot, vec3 scale, bee_bool rotate_global, mat4 model);
// sets all the uniforms for a shader
void set_shader_uniforms(material* mat);

// clears all internally used data, used for f.e. switching scenes
void renderer_clear_scene();

#ifdef EDITOR_ACT
// retrieve the entity id at the mouse position from the buffer cretated in render_scene_mouse_pick() 
int read_mouse_position_mouse_pick_buffer_color();

// submit a debug sphere to the renderer, these wont show up in the game
// but its quicker than using an entity for f.e. testing what point your calculation spat out
void debug_draw_sphere(vec3 pos, vec3 scale);
// submit a debug line to the renderer, these wont show up in the game
// f.e. nice for checking if mouse select or shooting via raycast works
void debug_draw_line(vec3 pos1, vec3 pos2);
// submit a debug cube to the renderer, these wont show up in the game
void debug_draw_cube(vec3 pos, vec3 scale);
// but its quicker than using an entity for f.e. testing what point your calculation spat out
#endif

// get the id of the current camera entity
int   get_camera_ent_id();
// pointer to an array holding the id's of all transparent entities
int** get_transparent_ids_ptr();
// get the length of get_transparent_ids_ptr()
int*  get_transparent_ids_len();
// pointer to an array holding the id's of all entities with a dir light 
int** get_dir_light_ids_ptr();
// get the length of get_dir_light_ids_ptr()
int*  get_dir_light_ids_len();
// pointer to an array holding the id's of all entities with a point light 
int** get_point_light_ids_ptr();
// get the length of get_point_light_ids_ptr()
int*  get_point_light_ids_len();
// pointer to an array holding the id's of all entities with a spot light 
int** get_spot_light_ids_ptr();
// get the length of get_spot_light_ids_ptr()
int*  get_spot_light_ids_len();

// set the id informing the engine which entity currently has the camera
// !!! use carefully !!!
void set_camera_ent_id(int id);

#ifdef EDITOR_ACT
// set play / pause state in editor
void set_gamestate(bee_bool play, bee_bool _hide_gizmos);
// return true: play false: paused / in editor
bee_bool get_gamestate();
// activate / deactivate all scripts
void set_all_scripts(bee_bool act);
#endif

// get how many draw calls there were last frame
int* get_draw_calls_per_frame();
// get how many vertices were rendered last frame
int* get_verts_per_frame();
// get how many triangles were rendered last frame
int* get_tris_per_frame();

// set any of the "render_setting" enum values 
void renderer_set(render_setting setting, bee_bool value);
// get any of the "render_setting" enum values 
bee_bool* renderer_get(render_setting setting);
// get the currently used exposure value
f32* get_exposure();
// get the color buffer, the hdr buffer all the objects get rendered into using lighting
u32 get_color_buffer();
// get the current background color
void get_bg_color(vec3 col);
// set the background color
void set_bg_color(vec3 col);

// @TODO: replace this
renderer_properties get_renderer_properties();
#endif