#ifdef EDITOR_ACT
#pragma once

#ifndef EDITOR_UI_H
#define EDITOR_UI_H


#include "global.h"
#include "asset_manager.h"
#include "object_data.h"


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

void ui_init();

void ui_update();

void ui_cleanup();

// void demo_window();
// void overview_window();

void properties_window();
// updates all relevant variables, related to the entity selection
void check_entity_selection();
// sets all variables necessary for deselcting the current entity
void deselect_entity();
// returns the currently in the editor ui selected entity id
int	 get_selected_entity();
void draw_entity_hierarchy_entity(int idx, int offset);
void draw_entity_hierarchy_type(int id);
void draw_transform_component(entity* ent);
void draw_mesh_component(entity* ent);
void draw_material_component(entity* ent);
void draw_physics_components(entity* ent);
void draw_camera_component(entity* ent);
void draw_light_component(entity* ent);
void properties_popups();

void pause_button_window();

void asset_browser_window();
void asset_browser_popups(texture* textures, mesh* meshes, gravity_script* scripts, material* materials, shader* shaders);

void add_shader_window();

void scene_context_window();

void source_code_window();
// activate the "source_code_window()" and provide it with the code to be displayed
void set_source_code_window(char* src);

void console_window();
// write a line of text to the in editor console
// inactive in play builds
void submit_txt_console(char* txt);

void error_popup_window();
// activate the "error_popup_window()" and give it a message
// "type" specifies what type of error it is usually "GENERAL_ERROR"
void set_error_popup(error_type type, char* msg);

void edit_asset_window();
// activate the "edit_asset_window()" and set the asset to be edited
// asset_ptr is a void* and accepts all asset types
// therefore the asset type has to be specified extra
void set_edit_asset_window(asset_type type, void* asset_ptr);

void drag_and_drop_import_window();
// activate the "drag_and_drop_import_window()" and set the file paths
void set_drag_and_drop_import_window(int path_count, char* paths[]);

// themes available to choose from using "set_style()"
typedef enum ui_theme { THEME_BLACK, THEME_WHITE, THEME_RED, THEME_BLUE, THEME_DARK, THEME_LIGHT_BLUE }ui_theme;
// set all color values in ctx->style according to the given "theme"
static void set_style(struct nk_context* ctx, enum ui_theme theme);

#endif
#else 
#pragma once

//
// this provides empty functions for some of the editor_ui.h functions
// this way i dont have to put "#ifdef EDITOR_ACT" statements around each of them
// 

#ifndef EDITOR_UI_H
#define EDITOR_UI_H


#include "global.h"


// during play-build this obviously doesnt do anything
void submit_txt_console(char* doesnt_show_up);

// during play-build this obviously doesnt do anything
void set_error_popup(error_type type, char* doesnt_show_up);

// during play-build this obviously doesnt do anything
void set_error_popup(error_type type, char* doesnt_show_up);

#endif
#endif
