#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include "global.h"
#include "types/object_data.h"

// all types of assets recognised by the engine
// "NOT_ASSET" is f.e. returned by "get_asset_type()" if the file isnt an asset
// "INTERNAL_ASSET" isnt based on a file type like .png or .obj but juts clarifies which assets are used by the engine itself
typedef enum asset_type { NOT_ASSET, TEXTURE_ASSET, MESH_ASSET, SCRIPT_ASSET, MATERIAL_ASSET, SHADER_ASSET, VERT_SHADER_ASSET, FRAG_SHADER_ASSET, INTERNAL_ASSET, SCENE_ASSET } asset_type;




void assetm_init();

// get the path to the asset folder
// f.e. "C:\project\assets" 
char* get_asset_dir();
// searches the asset folder for asset files like textures, meshes, etc.
// taken from: https://codeforwin.org/2018/03/c-program-to-list-all-files-in-a-directory-recursively.html
void search_dir(const char* dir_path);
// checks whether a file is an asset and registers it with the asset manager
void check_file(char* file_name, int file_name_len, char* dir_path);
// copies the given file into the asset folder and registers it with the asset manager
void add_file_to_project(char* file_path);
// loads the assets needed by the engine
void load_internal_assets();

void assetm_cleanup();

// get an array holding all internal assets names and its length
char** get_all_internals(int* internals_len);
// get the asset type by providing the files name
asset_type get_asset_type(char* file_name);
// check whether an asset is loaded or just registered
bee_bool check_asset_loaded(char* name);
// check if an asset is marked as an internal asset
bee_bool check_asset_internal(char* name);

// check whether a texture of the given name exists 
bee_bool check_texture_exists(const char* name);
// get the index of a texture in "texture_data[]" by name
int get_texture_idx(char* name);
// get the array holding all textures and its length
texture* get_all_textures(int* textures_len);
// get a specific texture
// returns a pink texture if texture name not found
texture  get_texture(const char* name);
// get a specific texture by its index in "texture_data[]"
// returns a pink texture if texture name not found
texture* get_texture_by_idx(int idx);
// get the path to the specified texture
char* get_texture_path(const char* name);
// get an array of all textures registered, not yet loaded, textures and its length
char** get_all_logged_textures(int* len);
// get a registered texture by its index
char* get_logged_texture(int idx);
// remove a registered texture, making it not longer available through "get_texture()"
void remove_logged_texture(char* name);
// register an asset with the asset manager, doesnt get loaded immediately
// becomes available through "get_texture()"
void log_texture(const char* path, const char* name);
// loads a registered texture
void create_texture(const char* name);

// check whether a mesh of the given name exists 
bee_bool check_mesh_exists(const char* name);
// get an array of all meshes registered, not yet loaded, textures and its length
char** get_all_logged_meshes(int* len);
// get a registered mesh by its index
char* get_logged_mesh(int idx);
// remove a registered mesh, making it not longer available through "get_mesh()"
void remove_logged_mesh(char* name);
// get the index of a mesh in "mesh_data[]" by name
int get_mesh_idx(char* name);
// get the array holding all meshes and its length
mesh* get_all_meshes(int* meshes_len);
// get a specific mesh
// returns an X shaped mesh if mesh name not found
mesh* get_mesh(const char* name);
// register an asset with the asset manager, doesnt get loaded immediately
// becomes available through "get_texture()"
void log_mesh(const char* path, const char* name);
// loads a registered mesh
void create_mesh(const char* name);
// manually add a mesh to the asset manager
rtn_code add_mesh(mesh m);

// check whether a script of the given name exists 
bee_bool check_script_exists(const char* name);
// get the index of a shader in "scripts_data[]" by name
int get_script_idx(char* name);
// get the array holding all scripts and its length
gravity_script* get_all_scripts(int* scripts_len);
// get a specific script
// returns empty script if script name not found
gravity_script* get_script(const char* name);
// loads a script from file
void create_script(const char* path, const char* name);

// check whether a material of the given name exists 
bee_bool check_material_exists(const char* name);
// get the index of a material in "materials_data[]" by name
int get_material_idx(char* name);
// create a material and add it to the asset manager
material* add_material(shader* s, texture dif_tex, texture spec_tex, bee_bool is_transparent, f32 shininess, vec2 tile, vec3 tint, bee_bool draw_backfaces, const char* name, bee_bool overwrite);
// create a material and add it to the asset manager
material* add_material_specific(shader* s, texture dif_tex, texture spec_tex, texture norm_tex, bee_bool is_transparent, f32 shininess, vec2 tile, vec3 tint, bee_bool draw_backfaces, int uniforms_len, uniform* uniforms, const char* name, bee_bool overwrite);
// get a specific material
// returns material with pink textures if material name not found
material* get_material(char* name);
// get the array holding all materials and its length
material* get_all_materials(int* materials_len);
// change a materials name
// !!! use this and not material.name = ... as that wont change the name in the asset manager
// !!! which is used f.e. for "get_material()"
void change_material_name(char* old_name, char* new_name);

// check whether a shader of the given name exists 
bee_bool check_shader_exists(const char* name);
// get the index of a shader in "shaders_data[]" by name
int get_shader_idx(char* name);
// create a shader and add it to the asset manager
shader* add_shader_specific(const char* vert_name, const char* frag_name, const char* name, bee_bool use_lighting, int uniforms_len, uniform_def* uniforms, bee_bool overwrite);
// create a shader and add it to the asset manager
shader* add_shader(const char* vert_name, const char* frag_name, const char* name, bee_bool overwrite);
// get a specific material
// returns shader with pink color if shader name not found
shader* get_shader(char* name);
// get a shader by its index in the "shader_data" array
// use "get_shader_idx()" to get a shaders idx
shader* get_shader_by_idx(int idx);
// get the array holding all shaders and its length
shader* get_all_shaders(int* shaders_len);
// get the length of the array holding all shaders
int		get_shaders_len();
// register an asset with the asset manager, doesnt get loaded immediately
// becomes available through "add_shader()"
void log_vert_file(const char* path, const char* name);
// register an asset with the asset manager, doesnt get loaded immediately
// becomes available through "add_shader()"
void log_frag_file(const char* path, const char* name);
// get the array holding all vertex shaders and its length
char** get_all_vert_file_names(int* vert_files_len);
// get the array holding all fragment shaders and its length
char** get_all_frag_file_names(int* frag_files_len);
// recompiles a shader, so the current version of all shader files gets used
void recomile_shader(const char* name);


// check whether a scene of the given name exists
bee_bool check_scene_exists(const char* name);
// get the length of the array holding all scenes
int   get_scenes_len();
// get path to the scene with given name
char* get_scene_path(const char* name);
// get the path to a scene by its index in "scene_paths[]"
char* get_scene_by_idx(int idx);
// register an asset with the asset manager, doesnt get loaded immediately
// becomes available through "get_scene_path()"
void  log_scene(const char* path, const char* name);


#endif
