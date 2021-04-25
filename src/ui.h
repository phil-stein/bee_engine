#pragma once
#ifndef UI_H
#define UI_H

#include "object_data.h"
#include "global.h"

typedef struct
{
	char* ent_name;
	enum bee_bool* rotate_global;
	enum bee_bool* has_model;
	enum bee_bool* has_light;

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
	u32* verts_len;
	u32* indices_len;
	enum bee_bool* mesh_indexed;
	enum bee_bool* mesh_visible;

	// material
	f32* shininess;
	f32* tile_x;
	f32* tile_y;

	f32* tint_r;
	f32* tint_g;
	f32* tint_b;
	
	char* dif_tex_name;
	char* spec_tex_name;
	bee_bool* is_transparent;

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


}entity_properties;

void ui_init();

void ui_update();

void ui_cleanup();

void demo_window();

void overview_window();

void properties_window(int ent_len);

void console_window();

void asset_browser_window();

// themes
enum theme { THEME_BLACK, THEME_WHITE, THEME_RED, THEME_BLUE, THEME_DARK };
static void set_style(struct nk_context* ctx, enum theme theme);

#endif
