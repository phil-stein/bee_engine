#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include "object_data.h"
#include "global.h"


typedef enum asset_type { NOT_ASSET, TEXTURE_ASSET, MESH_ASSET, SCRIPT_ASSET, MATERIAL_ASSET, SHADER_ASSET, VERT_SHADER_ASSET, FRAG_SHADER_ASSET, INTERNAL_ASSET, SCENE_ASSET } asset_type;




void assetm_init();

char* get_asset_dir();
// taken from: https://codeforwin.org/2018/03/c-program-to-list-all-files-in-a-directory-recursively.html
void search_dir(const char* dir_path);
void check_file(char* file_name, int file_name_len, char* dir_path);
void add_file_to_project(char* file_path);
void load_internal_assets();

void assetm_cleanup();


char** get_all_internals(int* internals_len);
asset_type get_asset_type(char* file_name);
bee_bool check_asset_loaded(char* name);
bee_bool check_asset_internal(char* name);

bee_bool check_texture_exists(const char* name);
int get_texture_idx(char* name);
texture* get_all_textures(int* textures_len);
texture  get_texture(const char* name);
texture* get_texture_by_idx(int idx);
char* get_texture_path(const char* name);
char** get_all_logged_textures(int* len);
char* get_logged_texture(int idx);
void remove_logged_texture(char* name);
// void add_logged_asset(asset_type type, char* asset_name);
void log_texture(const char* path, const char* name);
void create_texture(const char* name);

bee_bool check_mesh_exists(const char* name);
char** get_all_logged_meshes(int* len);
char* get_logged_mesh(int idx);
void remove_logged_mesh(char* name);
int get_mesh_idx(char* name);
mesh* get_all_meshes(int* meshes_len);
mesh* get_mesh(const char* name);
void log_mesh(const char* path, const char* name);
void create_mesh(const char* name);
rtn_code add_mesh(mesh m);

bee_bool check_script_exists(const char* name);
int get_script_idx(char* name);
gravity_script* get_all_scripts(int* scripts_len);
gravity_script* get_script(const char* name);
void create_script(const char* path, const char* name);

bee_bool check_material_exists(const char* name);
int get_material_idx(char* name);
material* add_material(shader* s, texture dif_tex, texture spec_tex, bee_bool is_transparent, f32 shininess, vec2 tile, vec3 tint, bee_bool draw_backfaces, const char* name, bee_bool overwrite);
material* add_material_specific(shader* s, texture dif_tex, texture spec_tex, bee_bool is_transparent, f32 shininess, vec2 tile, vec3 tint, bee_bool draw_backfaces, int uniforms_len, uniform* uniforms, const char* name, bee_bool overwrite);
material* get_material(char* name);
material* get_all_materials(int* materials_len);
void change_material_name(char* old_name, char* new_name);

bee_bool check_shader_exists(const char* name);
int get_shader_idx(char* name);
shader* add_shader_specific(const char* vert_name, const char* frag_name, const char* name, bee_bool use_lighting, int uniforms_len, uniform* uniforms);
shader* add_shader(const char* vert_name, const char* frag_name, const char* name);
shader* get_shader(char* name);
shader* get_all_shaders(int* shaders_len);
int		get_shaders_len();
void log_vert_file(const char* path, const char* name);
void log_frag_file(const char* path, const char* name);
char** get_all_vert_file_names(int* vert_files_len);
char** get_all_frag_file_names(int* frag_files_len);


bee_bool check_scene_exists(const char* name);
int   get_scenes_len();
char* get_scene_path(const char* name);
char* get_scene_by_idx(int idx);
void  log_scene(const char* path, const char* name);


#endif
