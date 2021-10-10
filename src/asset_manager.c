#include "asset_manager.h"

#include <direct.h>
#include "dirent/dirent.h"

#include "stb/stb_ds.h" // STB_DS_IMPLEMENTATION defined in renderer.c

#include "file_handler.h"
#include "str_util.h"
#include "shader.h"
#include "ui.h"


// ---- vars ----
// hashmaps & dyn.-arrays using stb_ds.h
#define ASSET_DIR_MAX_LEN 100
char asset_dir_path[ASSET_DIR_MAX_LEN];

// ---- textures ----
// value: index of texture in 'tex', key: name of texture 
struct { char* key;  int   value; }* textures	   = NULL;
int textures_len = 0;

// value: path to image, key: texture-index in 'textures'
struct { int   key;	 char* value; }* texture_paths = NULL; // turn this into an array, god damn ahhhh
int texture_paths_len = 0;

// array of holding textures
texture* texture_data = NULL;
int texture_data_len = 0;

// array holding the names of all textures not yet loaded
char** logged_textures = NULL;


// ---- meshes ----
// value: index of mesh in 'mesh_data', key: name of mesh
struct { char* key;  int   value; }*meshes = NULL;
int meshes_len = 0;

// value: path to mesh, key: mesh-index in 'meshes'
struct { int   key;	 char* value; }*meshes_paths = NULL; // turn this into an array, god damn ahhhh
int meshes_paths_len = 0;

// array of holding meshes
mesh* mesh_data = NULL;
int mesh_data_len = 0;

// array holding the names of all meshes not yet loaded
char** logged_meshes = NULL;


// ---- scripts ----
// value: index of script in 'script_data', key: name of script
struct { char* key;  int   value; }*scripts = NULL;
int scripts_len = 0;

// value: path to script, key: script-index in 'scripts'
struct { int   key;	 char* value; }*scripts_paths = NULL; // turn this into an array, god damn ahhhh
int scripts_paths_len = 0;

// array of holding scripts
gravity_script* scripts_data = NULL;
int scripts_data_len = 0;


// ---- materials ----
// value: index of script in 'material_data', key: name of material
struct { char* key;  int   value; }*materials = NULL;
int materials_len = 0;

// array of holding materials
material* materials_data = NULL;
int materials_data_len = 0;


// ---- shaders ----
// value: index of shader in 'shaders_data', key: name of shader
struct { char* key;  int   value; }*shaders = NULL;
int shaders_len = 0;

// array of holding shaders
shader* shaders_data = NULL;
int shaders_data_len = 0;

// ---- vert-files 
struct { char* key;  int   value; }*vert_files = NULL;
int vert_files_len = 0;

// value: path to vert_file, key: vert_file-index in 'vert_files'
struct { int   key;	 char* value; }*vert_files_paths = NULL;
int vert_files_paths_len = 0;

char** logged_vert_files = NULL;

// ---- frag-files 
struct { char* key;  int   value; }*frag_files = NULL;
int frag_files_len = 0;

// value: path to frag_file, key: frag_file-index in 'frag_files'
struct { int   key;	 char* value; }*frag_files_paths = NULL; 
int frag_files_paths_len = 0;

char** logged_frag_files = NULL;


// ---- scenes ----
// value: index of texture in 'scene_data', key: name of scene 
// struct { char* key;  int   value; }*scenes = NULL;
// int scenes_len = 0;

// value: path to scene-file, key: scene-name
struct { char*   key;	 char* value; }*scenes_paths = NULL;
int scenes_paths_len = 0;


// array of holding scenes
// scene* scene_data = NULL;
// int texture_data_len = 0;

// array holding the names of all textures not yet loaded
// char** logged_textures = NULL;



// ---- internal assets ----
char* internal_assets_names[] = {
									// ---- 0th element assets ----
								    // texture with index 0 in texture_data array 
									// this means this texture shows up when requested texture isnt found
									"missing_texture.png",
									
								    // mesh with index 0 in meshes_data array 
									// this means this mesh shows up when requested mesh isnt found
									"missing_mesh.obj",
									
									// scene with index 0 in scenes_paths array 
									// this means this scene shows up when requested scene isnt found
									// "default.scene",

									// ---- misc ----
									// needed for blank material, unlit material, etc.
									"blank.png",

									// ---- ui ----
									"mesh_icon.png",
									"script_icon.png",
									"shader_icon.png",
									"scene_icon.png",
									"logged_mesh_icon.png",
									"logged_texture_icon.png",

									// ---- skybox ----
									"top.jpg",
									"left.jpg",
									"back.jpg",
									"right.jpg",
									"front.jpg",
									"bottom.jpg",

									// ---- gizmos ----
									"camera.obj",
									"lightbulb.obj",
									"arrow_down.obj",
									"flashlight.obj",
};
int internal_assets_names_len = 18;
// bee_bool all_internal_assets_found = BEE_FALSE;



void assetm_init()
{
	// char* dir_path = get_asset_dir();

	// ---- get asset dir ----
	// load all assets in "proj\assets\"
	char* cwd = _getcwd(NULL, 0);
	// couldnt retrieve current working dir
	assert(cwd != NULL);
	char* dir_path = strcat(cwd, "\\assets");
	// couldnt concat cwd
	assert(dir_path != NULL);
	assert(strlen(dir_path) < ASSET_DIR_MAX_LEN);
	strcpy_s(asset_dir_path, ASSET_DIR_MAX_LEN, dir_path);


	printf("dir path: \"%s\"\n", asset_dir_path);
	search_dir(asset_dir_path, BEE_FALSE);

	// set default return if key doesn't exist
	shdefault(textures, -1);
	shdefault(meshes, -1);
	shdefault(shaders, -1);
	shdefault(materials, -1);
	shdefault(scripts, -1);
	shdefault(scenes_paths, -1);
}

char* get_asset_dir()
{

	return asset_dir_path;
}

void search_dir(const char* dir_path)
{
	char path[256];
	struct dirent* dp;
	DIR* dir = opendir(dir_path);
	// unable to open directory stream
	if (!dir) { return; }

	// recursively read the directory and its sub-directories
	while ((dp = readdir(dir)) != NULL)
	{
		// check that the file currently read isn't a directory
		if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
		{
			// printf("%s, \t[-1]%c, [-2]%c, [-3]%c, [-4]%c\n", dp->d_name, dp->d_name[dp->d_namlen - 1], dp->d_name[dp->d_namlen - 2], dp->d_name[dp->d_namlen - 3], dp->d_name[dp->d_namlen - 4]);
			
			// printf("-> %s\n", dp->d_name);
			check_file(dp->d_name, strlen(dp->d_name), dir_path);

			// construct new path from our base path
			strcpy(path, dir_path);
			strcat(path, "\\");
			strcat(path, dp->d_name);

			search_dir(path); // search recursively
		}
	}

	// close the stream
	closedir(dir);
}

void load_internal_assets()
{
	for (int i = 0; i < internal_assets_names_len; ++i)
	{
		asset_type type = get_asset_type(internal_assets_names[i]);
		int	name_len = strlen(internal_assets_names[i]);

		if (type == TEXTURE_ASSET)
		{
			get_texture(internal_assets_names[i]);
		}
		else if (type == MESH_ASSET)
		{
			get_mesh(internal_assets_names[i]);
		}
		/*
		if (internal_assets_names[i][name_len - 4] == '.' &&
			internal_assets_names[i][name_len - 3] == 'p' &&
			internal_assets_names[i][name_len - 2] == 'n' &&
			internal_assets_names[i][name_len - 1] == 'g')
		{
			get_texture(internal_assets_names[i]);
		}
		else if (internal_assets_names[i][name_len - 4] == '.' &&
				 internal_assets_names[i][name_len - 3] == 'j' &&
				 internal_assets_names[i][name_len - 2] == 'p' &&
				 internal_assets_names[i][name_len - 1] == 'g')
		{
			get_texture(internal_assets_names[i]);
		}
		else if (
			internal_assets_names[i][name_len - 4] == '.' &&
			internal_assets_names[i][name_len - 3] == 'o' &&
			internal_assets_names[i][name_len - 2] == 'b' &&
			internal_assets_names[i][name_len - 1] == 'j')
		{
			get_mesh(internal_assets_names[i]);
		}
		*/
	}

	add_mesh(make_plane_mesh());

}
void check_file(char* file_name, int file_name_len, char* dir_path)
{
	// check file extensions
	asset_type type = get_asset_type(file_name);

	if (type == NOT_ASSET)
	{ return; }
	// ---- textures ----
	if (type == TEXTURE_ASSET)
	{
		char dir_path_cpy[250];
		strcpy(dir_path_cpy, dir_path);
		char* t_path = strcat(dir_path_cpy, "\\"); // add the slash
		strcat(t_path, file_name); // add the name 

		// log the texture with the created path and the file name 
		log_texture(t_path, file_name);
	}
	// ---- models ----
	else if (type == MESH_ASSET)
	{
		char dir_path_cpy[250];
		strcpy(dir_path_cpy, dir_path);
		char* t_path = strcat(dir_path_cpy, "\\"); // add the slash
		strcat(t_path, file_name); // add the name 

		// log the mesh with the created path and the file name 
		log_mesh(t_path, file_name);
	}
	// ---- gravity ----
	else if (type == SCRIPT_ASSET)
	{
		char dir_path_cpy[250];
		strcpy(dir_path_cpy, dir_path);
		char* t_path = strcat(dir_path_cpy, "\\"); // add the slash
		strcat(t_path, file_name); // add the name 

		// create the script with the created path and the file name
		// no logging as all scripts get created immediately
		create_script(t_path, file_name);
	}
	// ---- vert-file ----
	else if (type == VERT_SHADER_ASSET)
	{
		char dir_path_cpy[250];
		strcpy(dir_path_cpy, dir_path);
		char* t_path = strcat(dir_path_cpy, "\\"); // add the slash
		strcat(t_path, file_name); // add the name 

		// log the mesh with the created path and the file name 
		log_vert_file(t_path, file_name);
	}
	// ---- frag-file ----
	else if (type == FRAG_SHADER_ASSET)
	{
		char dir_path_cpy[250];
		strcpy(dir_path_cpy, dir_path);
		char* t_path = strcat(dir_path_cpy, "\\"); // add the slash
		strcat(t_path, file_name); // add the name 

		// log the mesh with the created path and the file name 
		log_frag_file(t_path, file_name);
	}
	// ---- scene-file ----
	else if (type == SCENE_ASSET)
	{
		char dir_path_cpy[250];
		strcpy(dir_path_cpy, dir_path);
		char* t_path = strcat(dir_path_cpy, "\\"); // add the slash
		strcat(t_path, file_name); // add the name 

		// log the mesh with the created path and the file name 
		log_scene(t_path, file_name);
	}
}
void add_file_to_project(char* file_path)
{
	if (!file_exists_check(file_path))
	{ return; }

	// char path_cpy[124];
	char* name = str_trunc(str_find_last_of(file_path, "\\"), 1); // isolate name
	if (get_asset_type(name) == NOT_ASSET)
	{
		sprintf(stderr,    "[Error] File added to Project isn't an Asset.");
		submit_txt_console("[Error] File added to Project isn't an Asset.");
		return;
	}
	char* path_cpy = get_asset_dir();
	strcat(path_cpy, "\\");
	strcat(path_cpy, name);

	copy_file(file_path, path_cpy);
	printf("copied to: %s\n", path_cpy);

	char* path_cpy_no_name = str_trunc(path_cpy, (strlen(name) + 1) * -1); // remove name from path

	check_file(name, strlen(name), path_cpy_no_name); // logs file if it is an asset
}

void assetm_cleanup()
{
	// @TODO: should free all the name_cpy and path_cpy strings created in log_texture()
	// for (int i = 0; i < texture_paths_len; ++i)
	// {
	// 	char* ptr = hmget(texture_paths, i);
	// 	free(ptr);
	// }


	// free the allocated memory
	for (int i = 0; i < texture_data_len; ++i)
	{
		free_texture(&texture_data[i]);
	}
	shfree(textures);
	hmfree(texture_paths);
	arrfree(texture_data);
	arrfree(logged_textures);

	
	// free the allocated memory
	for (int i = 0; i < mesh_data_len; ++i)
	{
		free_mesh(&mesh_data[i]);
	}
	shfree(meshes);
	hmfree(meshes_paths);
	arrfree(mesh_data);
	arrfree(logged_meshes);

	// free the allocated memory
	// free all scripts
	for (int i = 0; i < scripts_data_len; ++i)
	{
		free_script(&scripts_data[i]);
	}
	shfree(scripts);
	hmfree(scripts_paths);
	arrfree(scripts_data);

	// free the allocated memory
	shfree(materials);
	arrfree(materials_data);

	// free the allocated memory
	for (int i = 0; i < shaders_data_len; ++i)
	{
		free_shader(&shaders_data[i]);
	}
	shfree(shaders);
	shfree(vert_files);
	hmfree(vert_files_paths);
	arrfree(logged_vert_files);
	shfree(frag_files);
	hmfree(frag_files_paths);
	arrfree(logged_frag_files);
	arrfree(shaders_data);
}

//
// ---- misc ----
//
char** get_all_internals(int* internals_len)
{
	*internals_len = internal_assets_names_len;
	return internal_assets_names;
}

asset_type get_asset_type(char* file_name)
{
	int file_name_len = strlen(file_name);
	// check file extensions
	// ---- textures ----
	if (// PNG
		file_name[file_name_len - 4] == '.' &&
		file_name[file_name_len - 3] == 'p' &&
		file_name[file_name_len - 2] == 'n' &&
		file_name[file_name_len - 1] == 'g' ||
		// JPEG
		file_name[file_name_len - 4] == '.' &&
		file_name[file_name_len - 3] == 'j' &&
		file_name[file_name_len - 2] == 'p' &&
		file_name[file_name_len - 1] == 'g' ||
		// BMP
		file_name[file_name_len - 4] == '.' &&
		file_name[file_name_len - 3] == 'b' &&
		file_name[file_name_len - 2] == 'm' &&
		file_name[file_name_len - 1] == 'p')
	{
		return TEXTURE_ASSET;
	}
	// ---- models ----
	else if (// Wavefront .obj
		file_name[file_name_len - 4] == '.' &&
		file_name[file_name_len - 3] == 'o' &&
		file_name[file_name_len - 2] == 'b' &&
		file_name[file_name_len - 1] == 'j' ||
		// GLTF .glb 
		file_name[file_name_len - 4] == '.' &&
		file_name[file_name_len - 3] == 'g' &&
		file_name[file_name_len - 2] == 'l' &&
		file_name[file_name_len - 1] == 'b' ||
		// FBX .fbx
		file_name[file_name_len - 4] == '.' &&
		file_name[file_name_len - 3] == 'f' &&
		file_name[file_name_len - 2] == 'b' &&
		file_name[file_name_len - 1] == 'x')
	{
		return MESH_ASSET;
	}
	// ---- gravity ----
	else if (file_name[file_name_len - 8] == '.' &&
		file_name[file_name_len - 7] == 'g' &&
		file_name[file_name_len - 6] == 'r' &&
		file_name[file_name_len - 5] == 'a' &&
		file_name[file_name_len - 4] == 'v' &&
		file_name[file_name_len - 3] == 'i' &&
		file_name[file_name_len - 2] == 't' &&
		file_name[file_name_len - 1] == 'y')
	{
		return SCRIPT_ASSET;
	}
	// ---- vert-file ----
	else if (file_name[file_name_len - 5] == '.' &&
		file_name[file_name_len - 4] == 'v' &&
		file_name[file_name_len - 3] == 'e' &&
		file_name[file_name_len - 2] == 'r' &&
		file_name[file_name_len - 1] == 't')
	{
		return VERT_SHADER_ASSET;
	}
	// ---- frag-file ----
	else if (file_name[file_name_len - 5] == '.' &&
		file_name[file_name_len - 4] == 'f' &&
		file_name[file_name_len - 3] == 'r' &&
		file_name[file_name_len - 2] == 'a' &&
		file_name[file_name_len - 1] == 'g')
	{
		return FRAG_SHADER_ASSET;
	}
	// ---- scene-file ----
	else if (file_name[file_name_len - 6] == '.' &&
			 file_name[file_name_len - 5] == 's' &&
			 file_name[file_name_len - 4] == 'c' &&
			 file_name[file_name_len - 3] == 'e' &&
			 file_name[file_name_len - 2] == 'n' &&
			 file_name[file_name_len - 1] == 'e')
	{
		return SCENE_ASSET;
	}
	else
	{
		return NOT_ASSET;
	}
}

bee_bool check_asset_loaded(char* name)
{
	asset_type type = get_asset_type(name);
	int i = 9999;
	switch (type)
	{
		case TEXTURE_ASSET:
			i = shget(textures, name);
			break;
		case MESH_ASSET:
			i = shget(meshes, name);
			break;
		case SCRIPT_ASSET:
			i = shget(scripts, name);
			break;
		case MATERIAL_ASSET:
			i = shget(materials, name);
			break;
		case SHADER_ASSET:
			i = shget(shaders, name);
			break;
		case VERT_SHADER_ASSET:
			i = shget(vert_files, name);
			break;
		case FRAG_SHADER_ASSET:
			i = shget(frag_files, name);
			break;
	}
	// get the index to the asset array from the hashmap

	// @TODO: add security check that the texture doesn't exist at all
	//		  for example make the 0th texture allways be all pink etc.

	// check if the asset hasn't been loaded yet 
	return i == 9999 ? BEE_FALSE : BEE_TRUE;
}

bee_bool check_asset_internal(char* name)
{
	for (int i = 0; i < internal_assets_names_len; ++i)
	{
		if (!strcmp(internal_assets_names[i], name))
		{
			return BEE_TRUE;
			// printf("internal asset: %s\n", name);
		}
	}
	return BEE_FALSE;
}


//
// ---- textures ----
// 

int get_texture_idx(char* name)
{
	int i = shget(textures, name);
	return i == -1 ? 0 : i;
}

texture* get_all_textures(int* textures_len)
{
	*textures_len = texture_data_len;
	return texture_data;
}

texture get_texture(const char* name)
{
	if (!check_texture_exists(name))
	{
		// assert(0);
	}
	// get the index to the tex array from the hashmap
	int i = shget(textures, name);
	// check if the texture hasn't been loaded yet 
	if (i == 9999 || i > texture_data_len) // 0 == 0
	{
		create_texture(name);
	}

	// retrieve texture from tex array
	return texture_data[shget(textures, name) == -1 ? 0 : shget(textures, name)];
}

texture* get_texture_by_idx(int idx)
{
	return &texture_data[idx];
}

char* get_texture_path(const char* name)
{
	// key for the path is the index of the texture in the hashmap
	int path_idx = shgeti(textures, name);
	char* path = hmget(texture_paths, path_idx);
	return path;
}

char** get_all_logged_textures(int* len)
{
	*len = arrlen(logged_textures);
	return logged_textures;
}
char* get_logged_texture(int idx)
{
	return logged_textures[idx];
}
void remove_logged_texture(char* name)
{
	for (int i = 0; i < arrlen(logged_textures); ++i)
	{
		if (!strcmp(logged_textures[i], name))
		{
			arrdel(logged_textures, i);
			return;
		}
	}
}

void log_texture(const char* path, const char* name)
{
	if (check_texture_exists(name))
	{
		printf("tried to add texture: \"%s\" but already existed\n", name);
		return;
	}

	// make a persistent copy of the passed name
	char* name_cpy = malloc( (strlen(name) +1) * sizeof(char));
	assert(name_cpy != NULL);
	strcpy(name_cpy, name);
	// printf("texture name copy: \"%s\"\n", name_cpy);
	
	// put that copy of the name into the hashmap as the key 
	// to a value of 9999 as that indicates the asset hasnt been loaded yet
	shput(textures, name_cpy, 9999);
	textures_len++;
	
	// make a persistent copy of the passed path
	char* path_cpy = malloc( (strlen(path) +1) * sizeof(char));
	assert(path_cpy != NULL);
	strcpy(path_cpy, path);
	// printf("texture path copy: \"%s\"\n", path_cpy);
	
	// put that copy of the path into the hashmap
	int i = shgeti(textures, name);
	hmput(texture_paths, i, path_cpy);
	texture_paths_len++;
	// printf("texture path: \"%s\"; texture name: \"%s\"; texture id: %d\n", path, name, i);

	// keep track of logged texture names
	arrput(logged_textures, name_cpy);
	
	// not freeing name_cpy and path_cpy as they need to be used in the future
}

void create_texture(const char* name)
{
	// key for the path is the index of the texture in the hashmap
	int path_idx = shgeti(textures, name);
	char* path = hmget(texture_paths, path_idx);
	// printf("-> create texture path: \"%s\"; path index: %d\n", path, path_idx);
	
	// copy name and path as passed name might be deleted
	char* name_cpy = malloc( (strlen(name) +1) * sizeof(char));
	assert(name_cpy != NULL);
	strcpy(name_cpy, name);
	bee_bool gamma_correct = str_find_last_of(name, "_dif") != NULL;
	texture t = texture_create_from_path(path, name_cpy, BEE_FALSE, gamma_correct);

	// put texture index in tex array into the value of the hashmap with the texture name as key 
	// and put the created texture into the tex array
	shput(textures, name_cpy, texture_data_len);
	arrput(texture_data, t);
	texture_data_len++;

	// remove texture from the logged_textures array
	remove_logged_texture(name);
}

bee_bool check_texture_exists(const char* name)
{
	return shget(textures, name) != -1 && shget(textures, name) != NULL;
}

// 
// ---- meshes ----
// 

char** get_all_logged_meshes(int* len)
{
	*len = arrlen(logged_meshes);
	return logged_meshes;
}
char* get_logged_mesh(int idx)
{
	return logged_meshes[idx];
}
void remove_logged_mesh(char* name)
{
	for (int i = 0; i < arrlen(logged_meshes); ++i)
	{
		if (!strcmp(logged_meshes[i], name))
		{
			arrdel(logged_meshes, i);
			return;
		}
	}
}

int get_mesh_idx(char* name)
{
	return shget(meshes, name);
}

mesh* get_all_meshes(int* meshes_len)
{
	*meshes_len = mesh_data_len;
	return mesh_data;
}

mesh* get_mesh(const char* name)
{
	// @TODO: add security check that the texture doesn't exist at all
	//		  for example make the 0th texture allways be all pink etc.
	if (!check_mesh_exists(name))
	{
		assert(0);
	}

	// get the index to the mesh_data array from the hashmap
	int i = shget(meshes, name);


	// check if the mesh hasn't been loaded yet 
	if (i == 9999 || i > mesh_data_len) // 0 == 0
	{
		create_mesh(name);
	}

	// retrieve mesh from mesh_data array
	return &mesh_data[shget(meshes, name) == -1 ? 0 : shget(meshes, name)];
}

void log_mesh(const char* path, const char* name)
{
	if (check_mesh_exists(name))
	{
		printf("tried to add mesh: \"%s\" but already existed\n", name);
		return;
	}

	// make a persistent copy of the passed name
	char* name_cpy = calloc(strlen(name) +1, sizeof(char));
	assert(name_cpy != NULL);
	strcpy(name_cpy, name);

	// put that copy of the name into the hashmap as the key 
	// to a value of 9999 as that indicates the asset hasnt been loaded yet
	shput(meshes, name_cpy, 9999);
	meshes_len++;

	// make a persistent copy of the passed path
	char* path_cpy = calloc(strlen(path) +1, sizeof(char));
	assert(path_cpy != NULL);
	strcpy(path_cpy, path);
	// printf("texture path copy: \"%s\"\n", path_cpy);

	// put that copy of the path into the hashmap
	int i = shgeti(meshes, name);
	hmput(meshes_paths, i, path_cpy);
	meshes_paths_len++;
	// printf("texture path: \"%s\"; texture name: \"%s\"; texture id: %d\n", path, name, i);

	// keep track of logged mesh names
	arrput(logged_meshes, name_cpy);

	// not freeing name_cpy and path_cpy as they need to be used in the future
}

void create_mesh(const char* name)
{
	// key for the path is the index of the mesh in the hashmap
	int path_idx = shgeti(meshes, name);
	char* path = hmget(meshes_paths, path_idx);

	mesh m = load_mesh(path);

	// put texture index in tex array into the value of the hashmap with the texture name as key 
	// and put the created texture into the tex array
	shput(meshes, name, mesh_data_len);
	arrput(mesh_data, m);
	mesh_data_len++;

	// remove mesh from the logged_meshes array
	for (int i = 0; i < arrlen(logged_meshes); ++i)
	{
		if (!strcmp(logged_meshes[i], name))
		{
			char* ptr = logged_meshes[i];
			arrdel(logged_meshes, i);
			break;
		}
	}
}

rtn_code add_mesh(mesh m)
{
	if (shget(meshes, m.name) != -1) // check if already exist
	{
		printf("[ERROR] Mesh added already exists\n");
		return BEE_ERROR;
	}
	shput(meshes, m.name, mesh_data_len);
	arrput(mesh_data, m);
	mesh_data_len++;
	return BEE_OK;
}

bee_bool check_mesh_exists(const char* name)
{
	return shget(meshes, name) != -1 && shget(meshes, name) != NULL;
}

// 
// ---- scripts ----
// 

int get_script_idx(char* name)
{
	return shget(scripts, name);
}

gravity_script* get_all_scripts(int* scripts_len)
{
	*scripts_len = scripts_data_len;
	return scripts_data;
}

gravity_script* get_script(const char* name)
{
	if (!check_script_exists(name))
	{
		assert(0);
	}
	// get the index to the mesh_data array from the hashmap
	int i = shget(scripts, name);

	// @TODO: add security check that the texture doesn't exist at all
	//		  for example make the 0th texture allways be all pink etc.

	// check if the mesh hasn't been loaded yet 
	// if (i == 9999 || i > scripts_data_len) // 0 == 0
	// {
	// 	create_script(name);
	// }

	// retrieve mesh from mesh_data array
	return &scripts_data[shget(scripts, name) == -1 ? 0 : shget(scripts, name)];
}

void create_script(const char* path, const char* name)
{
	if (check_script_exists(name))
	{
		printf("tried to add script: \"%s\" but already existed\n", name);
		return;
	}

	// make a persistent copy of the passed name
	char* name_cpy = calloc(strlen(name), sizeof(char));
	assert(name_cpy != NULL);
	strcpy(name_cpy, name);

	// make a persistent copy of the passed path
	char* path_cpy = calloc(strlen(path), sizeof(char));
	assert(path_cpy != NULL);
	strcpy(path_cpy, path);
	// printf("texture path copy: \"%s\"\n", path_cpy);

	// put that copy of the path into the hashmap
	int i = shgeti(scripts, name);
	hmput(scripts_paths, i, path_cpy);
	scripts_paths_len++;

	// key for the path is the index of the texture in the hashmap
	// int path_idx = shgeti(scripts, name);
	// char* path = hmget(scripts_paths, path_idx);

	// printf("script path: \"%s\"\n", path_cpy);
	gravity_script script = make_script(path_cpy);

	// put texture index in tex array into the value of the hashmap with the texture name as key 
	// and put the created texture into the tex array
	shput(scripts, name_cpy, scripts_data_len);
	scripts_len++;
	arrput(scripts_data, script);
	scripts_data_len++;

	// not freeing name_cpy and path_cpy as they need to be used in the future
}

bee_bool check_script_exists(const char* name)
{
	return shget(scripts, name) != -1 && shget(scripts, name) != NULL;
}

//
// ---- materials ----
// 

int get_material_idx(char* name)
{
	return shget(materials, name);
}

material* add_material(shader* s, texture dif_tex, texture spec_tex, bee_bool is_transparent, f32 shininess,
	vec2 tile, vec3 tint, bee_bool draw_backfaces, const char* name, bee_bool overwrite)
{
	return add_material_specific(s, dif_tex, spec_tex, get_texture("blank.png"), is_transparent, shininess,
								 tile, tint, draw_backfaces, 0, NULL, name, overwrite);
}

material* add_material_specific(shader* s, texture dif_tex, texture spec_tex, texture norm_tex, bee_bool is_transparent, f32 shininess, 
					   vec2 tile, vec3 tint, bee_bool draw_backfaces, int uniforms_len, uniform* uniforms, const char* name, bee_bool overwrite)
{
	if (shget(materials, name) != -1) // check if already exist
	{
		// printf("[ERROR] Material \"%s\" already exists\n", name);
		if (overwrite)
		{
			material* m = get_material(name);
			m->shader = s;
			m->dif_tex = dif_tex;
			m->spec_tex = spec_tex;
			m->norm_tex = norm_tex;
			m->is_transparent = is_transparent;
			m->shininess = shininess;
			glm_vec2_copy(tile, m->tile);
			glm_vec2_copy(tint, m->tint);
			m->draw_backfaces = draw_backfaces;
			// printf(" -> Material \"%s\" overwritten\n", name);
		}
		return get_material(name);
	}
	// make a persistent copy of the passed name
	char* name_cpy = malloc((strlen(name) +1) * sizeof(char)); 
	assert(name_cpy != NULL);
	strcpy(name_cpy, name);

	material mat = make_material(s, dif_tex, spec_tex, norm_tex, is_transparent, shininess, tile, tint, draw_backfaces, uniforms_len, uniforms, name_cpy);

	shput(materials, name_cpy, materials_data_len);
	materials_len++;
	arrput(materials_data, mat);
	materials_data_len++;

	return &materials_data[shget(materials, name_cpy)];
}

material* get_material(char* name)
{
	// @TODO: add security check that the texture doesn't exist at all
	//		  for example make the 0th texture allways be all pink etc.
	if (!check_material_exists(name))
	{
		assert(0);
	}

	// retrieve mesh from mesh_data array
	return &materials_data[shget(materials, name) == -1 ? 0 : shget(materials, name)];
}

material* get_all_materials(int* materials_len)
{
	*materials_len = materials_data_len;
	return materials_data;
}

// @TODO: doesnt work yet
void change_material_name(char* old_name, char* new_name)
{
	// materials[selected_material].name = calloc(strlen(name_edit_buffer) + 1, sizeof(char));
	(get_material(old_name))->name = calloc(strlen(new_name) + 1, sizeof(char));
	assert((get_material(old_name))->name != NULL);
	// copy name so its persistent
	char* name_cpy = malloc((strlen(new_name) +1) * sizeof(char));
	assert(name_cpy != NULL);
	strcpy(name_cpy, new_name);
	strcpy((get_material(old_name))->name, name_cpy);

	int idx = get_material_idx(old_name);
	shput(materials, name_cpy, idx);
	shdel(materials, old_name);
}

bee_bool check_material_exists(const char* name)
{
	return shget(materials, name) != -1;
}


//
// ---- shaders ----
// 

int get_shader_idx(char* name)
{
	return shget(shaders, name);
}

shader* add_shader_specific(const char* vert_name, const char* frag_name, const char* name, bee_bool use_lighting, int uniform_defs_len, uniform_type* uniform_defs)
{
	if (shget(shaders, name) != -1) // check if already exist
	{
		printf("[ERROR] Shader \"%s\" already exists\n", name);
		return get_shader(name);
	}

	// key for the path is the index of the file in the hashmap
	int path_idx = shgeti(vert_files, vert_name);
	char* vert_path = hmget(vert_files_paths, path_idx);
	path_idx = shgeti(frag_files, frag_name);
	char* frag_path = hmget(frag_files_paths, path_idx);

	// make a persistent copy of the passed name
	char* name_cpy = calloc(strlen(name) +1, sizeof(char));
	assert(name_cpy != NULL);
	strcpy(name_cpy, name);

	shader s = create_shader_from_file(vert_path, frag_path, name_cpy);
	s.vert_name = vert_name;
	s.frag_name = frag_name;
	s.use_lighting		= use_lighting;
	s.uniform_defs_len  = uniform_defs_len;
	s.uniform_defs		= uniform_defs;

	shput(shaders, name_cpy, shaders_data_len);
	shaders_len++;
	arrput(shaders_data, s);
	shaders_data_len++;

	return &shaders_data[shget(shaders, name)];
}

shader* add_shader(const char* vert_name, const char* frag_name, const char* name)
{
	return add_shader_specific(vert_name, frag_name, name, BEE_TRUE, 0, NULL);
}

shader* get_shader(char* name)
{
	// @TODO: add security check that the texture doesn't exist at all
	//		  for example make the 0th texture allways be all pink etc.
	if (!check_shader_exists(name))
	{
		assert(0);
	}

	// retrieve mesh from mesh_data array
	return &shaders_data[shget(shaders, name) == -1 ? 0 : shget(shaders, name)];
}

shader* get_all_shaders(int* shaders_len)
{
	*shaders_len = shaders_data_len;
	return shaders_data;
}
int get_shaders_len()
{
	return shaders_data_len;
}

char** get_all_vert_file_names(int* vert_files_len)
{
	*vert_files_len = arrlen(logged_vert_files);
	return logged_vert_files;
}

char** get_all_frag_file_names(int* frag_files_len)
{
	*frag_files_len = arrlen(logged_frag_files);
	return logged_frag_files;
}

void log_vert_file(const char* path, const char* name)
{
	// make a persistent copy of the passed name
	char* name_cpy = calloc(strlen(name) +1, sizeof(char));
	assert(name_cpy != NULL);
	strcpy(name_cpy, name);
	// printf("texture name copy: \"%s\"\n", name_cpy);

	// put that copy of the name into the hashmap as the key 
	// to a value of 9999 as that indicates the asset hasnt been loaded yet
	shput(vert_files, name_cpy, 9999);
	vert_files_len++;

	// make a persistent copy of the passed path
	char* path_cpy = calloc(strlen(path) +1, sizeof(char));
	assert(path_cpy != NULL);
	strcpy(path_cpy, path);

	// printf("vert_file path: \"%s\"\n", path);

	// put that copy of the path into the hashmap
	int i = shgeti(vert_files, name);
	hmput(vert_files_paths, i, path_cpy);
	vert_files_paths_len++;

	// "remember" the name
	arrput(logged_vert_files, name_cpy);

	// not freeing name_cpy and path_cpy as they need to be used in the future
}

void log_frag_file(const char* path, const char* name)
{
	// make a persistent copy of the passed name
	char* name_cpy = calloc(strlen(name) +1, sizeof(char));
	assert(name_cpy != NULL);
	strcpy(name_cpy, name);
	// printf("texture name copy: \"%s\"\n", name_cpy);

	// put that copy of the name into the hashmap as the key 
	// to a value of 9999 as that indicates the asset hasnt been loaded yet
	shput(frag_files, name_cpy, 9999);
	frag_files_len++;

	// make a persistent copy of the passed path
	char* path_cpy = calloc(strlen(path) +1, sizeof(char));
	assert(path_cpy != NULL);
	strcpy(path_cpy, path);

	// put that copy of the path into the hashmap
	int i = shgeti(frag_files, name);
	hmput(frag_files_paths, i, path_cpy);
	frag_files_paths_len++;

	// "remember" the name
	arrput(logged_frag_files, name_cpy);

	// not freeing name_cpy and path_cpy as they need to be used in the future
}

bee_bool check_shader_exists(const char* name)
{
	return shget(shaders, name) != -1; // && shget(shaders, name) != NULL;
}

// @TODO: check_vert_file_exists & check_frag_file_exists

void hot_reload_shader(const char* name)
{
	if (!check_shader_exists(name))
	{
		printf("[ERROR] Shader \"%s\" cant be reloaded it doesnt exist.\n");
		return;
	}

	shader* s = get_shader(name);

	// key for the path is the index of the file in the hashmap
	int path_idx = shgeti(vert_files, s->vert_name);
	char* vert_path = hmget(vert_files_paths, path_idx);
	path_idx = shgeti(frag_files, s->frag_name);
	char* frag_path = hmget(frag_files_paths, path_idx);

	// make a persistent copy of the passed name
	// char* name_cpy = calloc(strlen(name) + 1, sizeof(char));
	// assert(name_cpy != NULL);
	// strcpy(name_cpy, name);

	shader s_new = create_shader_from_file(vert_path, frag_path, s->name);
	s_new.vert_name = s->vert_name;
	s_new.frag_name = s->frag_name;
	s_new.use_lighting = s->use_lighting;
	s_new.uniform_defs_len = s->uniform_defs_len;
	s_new.uniform_defs = NULL;
	for (int i = 0; i < s_new.uniform_defs_len; ++i)
	{
		arrput(s_new.uniform_defs, s->uniform_defs[i]);
	}

	shaders_data[shget(shaders, name) == -1 ? 0 : shget(shaders, name)] = s_new;
}

//
// ---- scenes ----
//

int get_scenes_len()
{
	return scenes_paths_len;
}

char* get_scene_path(const char* name)
{
	if (!check_scene_exists(name))
	{
		// assert(0);
	}
	return shget(scenes_paths, name) == -1 ? 0 : shget(scenes_paths, name);
}

char* get_scene_by_idx(int idx)
{

	return scenes_paths[idx].key;
}

void log_scene(const char* path, const char* name)
{
	if (check_scene_exists(name)) 
	{ 
		printf("tried to add scene: \"%s\" but already existed\n", name);  
		return; 
	}

	// make a persistent copy of the passed name
	char* name_cpy = malloc((strlen(name) + 1) * sizeof(char));
	assert(name_cpy != NULL);
	strcpy(name_cpy, name);
	char* path_cpy = malloc((strlen(path) + 1) * sizeof(char));
	assert(path_cpy != NULL);
	strcpy(path_cpy, path);

	shput(scenes_paths, name_cpy, path_cpy);
	scenes_paths_len++;

	// not freeing name_cpy and path_cpy as they need to be used in the future
}

bee_bool check_scene_exists(const char* name)
{
	return shget(scenes_paths, name) != -1 && shget(scenes_paths, name) != NULL;
}
