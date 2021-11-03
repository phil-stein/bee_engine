#pragma once

#ifndef EDITOR_UI_H
#define EDITOR_UI_H


#include "global.h"
#include "files/asset_manager.h"

#ifdef EDITOR_ACT


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
// drop-down option at the top of the window, with f.e. "Scene -> Save"
void draw_properties_menu_bar();
// play / pause button for gamestate, and the settings like "Hide UI", etc.
void draw_play_pause_and_settings();
// scene properties, like "Use MSAA" also diagnostics, like "Draw Calls" / "Render Stages" and debug, like "Wireframe"
void draw_scene_properties();
// left side of the entity hierarchy, with selectable labels
void draw_entity_hierarchy_entity(int idx, int offset);
// right side of the entity hierarchy, with type description
void draw_entity_hierarchy_type(int id);
// ui for the transform component, used in "properties_window()"
void draw_transform_component(entity* ent);
// ui for the mesh component, used in "properties_window()"
void draw_mesh_component(entity* ent);
// ui for the material component, used in "properties_window()"
void draw_material_component(entity* ent);
// ui for the physics component, used in "properties_window()"
void draw_physics_components(entity* ent);
// ui for the camera component, used in "properties_window()"
void draw_camera_component(entity* ent);
// ui for the light component, used in "properties_window()"
void draw_light_component(entity* ent);
// the popups used in "properties_window()", seperated so the window doesn't get de-focused when popups are interacted with
void properties_popups();

// window used in play-mode with the "Pause" button
void pause_button_window();

// asset browser ui
void asset_browser_window();
// the popups used in "asset_browser_window()", seperated so the window doesn't get de-focused when popups are interacted with
// args are the arrays from "asset_manager.c" retrieved using "get_all_[asset type]s()"
void asset_browser_popups(texture* textures, mesh* meshes, gravity_script* scripts, material* materials, shader* shaders);

// ui to choose the vert / frag shader for new shader
void add_shader_window();

// ui to choose name for scene when saving using "Scene -> Save As" in "draw_properties_menu_bar()"
void scene_context_window();

// window showing the source code supplied via "set_source_code_window()"
// !!! dont call this yourself use "set_source_code_window()" to activate it
void source_code_window();
// activate the "source_code_window()" and provide it with the code to be displayed
void set_source_code_window(char* src);

// window containing the messages supplied via "submit_txt_console()"
void console_window();
// write a line of text to the in editor console
// inactive in play builds
void submit_txt_console(char* txt);

// window showing error messages set via "set_error_popup()"
// !!! dont call this yourself use "set_error_popup()" to activate it
void error_popup_window();
// activate the "error_popup_window()" and give it a message
// "type" specifies what type of error it is usually "GENERAL_ERROR"
void set_error_popup(error_type type, char* msg);

// ui to edit an asset specified over "set_edit_asset_window()"
// !!! dont call this yourself use "set_edit_asset_window()" to activate it
void edit_asset_window();
// activate the "edit_asset_window()" and set the asset to be edited
// asset_ptr is a void* and accepts all asset types
// therefore the asset type has to be specified extra
void set_edit_asset_window(asset_type type, void* asset_ptr);

// show assets dragged onto window, activated by "set_drag_and_drop_import_window()"
// !!! dont call this yourself use "set_drag_and_drop_import_window()" to activate it
void drag_and_drop_import_window();
// activate the "drag_and_drop_import_window()" and set the file paths
void set_drag_and_drop_import_window(int path_count, char* paths[]);

// get the bool specifing whether the move gizmo should be used
bee_bool get_show_move_gizmo();

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

// during play-build this obviously doesnt do anything
void submit_txt_console(char* doesnt_show_up);

// during play-build this obviously doesnt do anything
void set_error_popup(error_type type, char* doesnt_show_up);

// during play-build this obviously doesnt do anything
void set_error_popup(error_type type, char* doesnt_show_up);

#endif
