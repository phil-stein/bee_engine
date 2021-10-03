#pragma once
#ifndef UI_H
#define UI_H

#include "asset_manager.h"
#include "object_data.h"
#include "global.h"

typedef enum error_type
{
	GENERAL_ERROR,
	GRAVITY_ERROR
}error_type;

typedef struct
{
	bee_bool handled;
	float pos[2];
	asset_type type;
	int asset_idx;

}asset_drop;

typedef struct
{
	bee_bool handled;
	bee_bool assigned;
	float pos[2];
	int child_idx;
	int parent_idx;

}entity_drop;

typedef struct
{
	char* ent_name;
	bee_bool* rotate_global;
	bee_bool* has_model;
	bee_bool* has_light;
	bee_bool* has_trans;

	// transform
	f32* pos_x;
	f32* pos_y;
	f32* pos_z;

	f32* rot_x;
	f32* rot_y;
	f32* rot_z;
	
	f32* scale_x;
	f32* scale_y;
	f32* scale_z;

	// mesh
	mesh* mesh;
	char* mesh_name;
	u32* verts_len;
	u32* indices_len;
	enum bee_bool* mesh_indexed;
	enum bee_bool* mesh_visible;

	// material
	material* mat;
	char* material_name;
	f32* shininess;
	f32* tile_x;
	f32* tile_y;

	f32* tint_r;
	f32* tint_g;
	f32* tint_b;
	
	char* dif_tex_name;
	char* spec_tex_name;
	u32* dif_tex_handle;
	u32* spec_tex_handle;
	bee_bool* is_transparent;

	char* vert_source;
	char* frag_source;


	// light
	enum light_type* _light_type;

	f32* ambient_r;
	f32* ambient_g;
	f32* ambient_b;
	
	f32* diffuse_r;
	f32* diffuse_g;
	f32* diffuse_b;
	
	f32* specular_r;
	f32* specular_g;
	f32* specular_b;

	// -----------------

	// for dir & spot light
	f32* direction_x;
	f32* direction_y;
	f32* direction_z;

	f32* constant;	// for point & spot light
	f32* linear;		// for point & spot light
	f32* quadratic;	// for point & spot light

	// spot light
	f32* cut_off;
	f32* outer_cut_off;

	// -----------------

	int* scripts_len;
	gravity_script** scripts;


}entity_properties;

void ui_init();

void ui_update();

void ui_cleanup();

void demo_window();

void overview_window();

void properties_window(int ent_len);
void draw_entity_hierachy_entity(int idx, int offset);
void pause_button_window();

void console_window();

void asset_browser_window();


void submit_txt_console(char* txt);

void error_popup_window();

void set_error_popup(error_type type, char* msg);

void source_code_window();

void add_shader_window();

void set_source_code_window(char* src);

// themes
enum theme { THEME_BLACK, THEME_WHITE, THEME_RED, THEME_BLUE, THEME_DARK };
static void set_style(struct nk_context* ctx, enum theme theme);

#endif
