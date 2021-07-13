// #pragma once
#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include "global.h"
#include "object_data.h"


typedef enum asset_type { TEXTURE_ASSET, MESH_ASSET, SCRIPT_ASSET } asset_type;


void add_logged_asset(asset_type type, char* asset_name);
void remove_logged_asset();


void assetm_init();

// taken from: https://codeforwin.org/2018/03/c-program-to-list-all-files-in-a-directory-recursively.html
void search_dir(const char* dir_path);

void assetm_cleanup();


int get_texture_idx(char* name);

texture* get_all_textures(int* textures_len);
texture get_texture(const char* name);
char** get_all_logged_textures(int* len);
char* get_logged_texture(int idx);

void log_texture(const char* path, const char* name);
void create_texture(const char* name);


char** get_all_logged_meshes(int* len);
char* get_logged_mesh(int idx);

int get_mesh_idx(char* name);

mesh* get_all_meshes(int* meshes_len);
mesh* get_mesh(const char* name);

void log_mesh(const char* path, const char* name);
void create_mesh(const char* name);


int get_script_idx(char* name);

gravity_script* get_all_scripts(int* scripts_len);
gravity_script* get_script(const char* name);

void create_script(const char* path, const char* name);


material* add_material(u32 shader, texture dif_tex, texture spec_tex, bee_bool is_transparent, f32 shininess, vec2 tile, vec3 tint, const char* name);

material* get_material(char* name);

material* get_all_materials(int* materials_len);

#endif
