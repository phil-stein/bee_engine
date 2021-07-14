#include "asset_manager.h"

#include <direct.h>
// #include <sys/types.h>

#include "stb/stb_ds.h" // STB_DS_IMPLEMENTATION defined in renderer.c
#include "dirent/dirent.h"

#include "file_handler.h"
#include "shader.h"


// ---- vars ----
// hashmaps & dyn.-arrays using stb_ds.h
// prob. should replace these with something faster sometime [I mean maybe but it's actually pretty fast]

// ---- textures ----
// value: index of texture in 'tex', key: name of texture 
struct { char* key;  int   value; }* textures	   = NULL;
int textures_len = 0;

// value: path to image, key: texture-index in 'textures'
struct { int   key;	 char* value; }* texture_paths = NULL; // turn this into an array, god damn ahhhh
int texture_paths_len = 0;

// array of holding textures
texture* texture_data = NULL;
int texure_data_len = 0;

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

// ---- frag-files 
struct { char* key;  int   value; }*frag_files = NULL;
int frag_files_len = 0;

// value: path to frag_file, key: frag_file-index in 'frag_files'
struct { int   key;	 char* value; }*frag_files_paths = NULL; 
int frag_files_paths_len = 0;


void assetm_init()
{
	// load all assets in "proj\assets\"
	char* cwd = _getcwd(NULL, 0);
	// couldnt retrieve current working dir
	assert(cwd != NULL);
	char* dir_path = strcat(cwd, "\\assets");
	// couldnt concat cwd
	assert(dir_path != NULL);

	search_dir(dir_path);

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

			// check file extensions
			if (dp->d_name[dp->d_namlen - 4] == '.' &&
				dp->d_name[dp->d_namlen - 3] == 'p' &&
				dp->d_name[dp->d_namlen - 2] == 'n' &&
				dp->d_name[dp->d_namlen - 1] == 'g')
			{
				// printf("^---- PNG ----^\n");
				char* dir_path_cpy[250]; 
				strcpy(dir_path_cpy, dir_path);
				char* t_path = strcat(dir_path_cpy, "\\"); // add the slash
				strcat(t_path, dp->d_name); // add the name 

				// log the texture with the created path and the file name 
				log_texture(t_path, dp->d_name);
			}
			else if (dp->d_name[dp->d_namlen - 4] == '.' &&
					 dp->d_name[dp->d_namlen - 3] == 'o' &&
					 dp->d_name[dp->d_namlen - 2] == 'b' &&
					 dp->d_name[dp->d_namlen - 1] == 'j')
			{
				// printf("^---- PNG ----^\n");
				char* dir_path_cpy[250];
				strcpy(dir_path_cpy, dir_path);
				char* t_path = strcat(dir_path_cpy, "\\"); // add the slash
				strcat(t_path, dp->d_name); // add the name 

				// log the mesh with the created path and the file name 
				log_mesh(t_path, dp->d_name);
			}
			else if (dp->d_name[dp->d_namlen - 8] == '.' &&
					 dp->d_name[dp->d_namlen - 7] == 'g' &&
					 dp->d_name[dp->d_namlen - 6] == 'r' &&
					 dp->d_name[dp->d_namlen - 5] == 'a' &&
					 dp->d_name[dp->d_namlen - 4] == 'v' &&
					 dp->d_name[dp->d_namlen - 3] == 'i' &&
					 dp->d_name[dp->d_namlen - 2] == 't' &&
					 dp->d_name[dp->d_namlen - 1] == 'y')
			{
				// printf("^---- PNG ----^\n");
				char* dir_path_cpy[250];
				strcpy(dir_path_cpy, dir_path);
				char* t_path = strcat(dir_path_cpy, "\\"); // add the slash
				strcat(t_path, dp->d_name); // add the name 

				// create the script with the created path and the file name
				// no logging as all scripts get created immediately
				create_script(t_path, dp->d_name);
			}
			else if (dp->d_name[dp->d_namlen - 5] == '.' &&
					 dp->d_name[dp->d_namlen - 4] == 'v' &&
					 dp->d_name[dp->d_namlen - 3] == 'e' &&
					 dp->d_name[dp->d_namlen - 2] == 'r' &&
					 dp->d_name[dp->d_namlen - 1] == 't')
			{
				// printf("^---- PNG ----^\n");
				char* dir_path_cpy[250];
				strcpy(dir_path_cpy, dir_path);
				char* t_path = strcat(dir_path_cpy, "\\"); // add the slash
				strcat(t_path, dp->d_name); // add the name 

				// log the mesh with the created path and the file name 
				log_vert_file(t_path, dp->d_name);
			}
			else if (dp->d_name[dp->d_namlen - 5] == '.' &&
					 dp->d_name[dp->d_namlen - 4] == 'f' &&
					 dp->d_name[dp->d_namlen - 3] == 'r' &&
					 dp->d_name[dp->d_namlen - 2] == 'a' &&
					 dp->d_name[dp->d_namlen - 1] == 'g')
			{
				char* dir_path_cpy[250];
				strcpy(dir_path_cpy, dir_path);
				char* t_path = strcat(dir_path_cpy, "\\"); // add the slash
				strcat(t_path, dp->d_name); // add the name 

				// log the mesh with the created path and the file name 
				log_frag_file(t_path, dp->d_name);
			}

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

void assetm_cleanup()
{
	// @TODO: should free all the name_cpy and path_cpy strings created in log_texture()
	// for (int i = 0; i < texture_paths_len; ++i)
	// {
	// 	char* ptr = hmget(texture_paths, i);
	// 	free(ptr);
	// }


	// free the allocated memory
	shfree(textures);
	hmfree(texture_paths);
	arrfree(texture_data);

	// free the allocated memory
	shfree(meshes);
	hmfree(meshes_paths);
	arrfree(mesh_data);
	
	// free all scripts
	for (int i = 0; i < scripts_data_len; ++i)
	{
		free_script(&scripts_data[i]);
	}

	// free the allocated memory
	shfree(scripts);
	hmfree(scripts_paths);
	arrfree(scripts_data);

	// free the allocated memory
	shfree(materials);
	arrfree(materials_data);

	// free the allocated memory
	shfree(shaders);
	arrfree(shaders_data);
}


//
// ---- textures ----
// 

int get_texture_idx(char* name)
{
	return shget(textures, name);
}

texture* get_all_textures(int* textures_len)
{
	*textures_len = texure_data_len;
	return texture_data;
}

texture get_texture(const char* name)
{
	// get the index to the tex array from the hashmap
	int i = shget(textures, name);

	// @TODO: add security check that the texture doesn't exist at all
	//		  for example make the 0th texture allways be all pink etc.

	// check if the texture hasn't been loaded yet 
	if (i == 9999 || i > texure_data_len) // 0 == 0
	{
		create_texture(name);
	}

	// retrieve texture from tex array
	return texture_data[shget(textures, name)];
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

void log_texture(const char* path, const char* name)
{
	// make a persistent copy of the passed name
	char* name_cpy = calloc(strlen(name) +1, sizeof(char));
	assert(name_cpy != NULL);
	strcpy(name_cpy, name);
	// printf("texture name copy: \"%s\"\n", name_cpy);
	
	// put that copy of the name into the hashmap as the key 
	// to a value of 9999 as that indicates the asset hasnt been loaded yet
	shput(textures, name_cpy, 9999);
	textures_len++;
	
	// make a persistent copy of the passed path
	char* path_cpy = calloc(strlen(path) +1, sizeof(char));
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
	char* name_cpy = calloc(strlen(name) +1, sizeof(char));
	assert(name_cpy != NULL);
	strcpy(name_cpy, name);
	texture t = texture_create_from_path(path, name_cpy, BEE_FALSE);

	// put texture index in tex array into the value of the hashmap with the texture name as key 
	// and put the created texture into the tex array
	shput(textures, name, texure_data_len);
	arrput(texture_data, t);
	texure_data_len++;

	// remove texture from the logged_textures array
	for (int i = 0; i < arrlen(logged_textures); ++i)
	{
		if (!strcmp(logged_textures[i], name))
		{
			char* ptr = logged_textures[i];
			arrdel(logged_textures, i);
			break;
		}
	}
}

// 
// ---- meshes ----
// 


char** get_all_logged_meshes(int* len)
{

	printf("logged-meshes assetmanager:\n");
	for (int i = 0; i < arrlen(logged_meshes); ++i)
	{
		printf(" -> %s\n", logged_meshes[i]);
	}

	*len = arrlen(logged_meshes);
	return logged_meshes;
}
char* get_logged_mesh(int idx)
{
	return logged_meshes[idx];
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
	// get the index to the mesh_data array from the hashmap
	int i = shget(meshes, name);

	// @TODO: add security check that the texture doesn't exist at all
	//		  for example make the 0th texture allways be all pink etc.

	// check if the mesh hasn't been loaded yet 
	if (i == 9999 || i > mesh_data_len) // 0 == 0
	{
		create_mesh(name);
	}

	// retrieve mesh from mesh_data array
	return &mesh_data[shget(meshes, name)];
}

void log_mesh(const char* path, const char* name)
{
	// make a persistent copy of the passed name
	char* name_cpy = calloc(strlen(name), sizeof(char));
	assert(name_cpy != NULL);
	strcpy(name_cpy, name);

	// put that copy of the name into the hashmap as the key 
	// to a value of 9999 as that indicates the asset hasnt been loaded yet
	shput(meshes, name_cpy, 9999);
	meshes_len++;

	// make a persistent copy of the passed path
	char* path_cpy = calloc(strlen(path), sizeof(char));
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
	return &scripts_data[shget(scripts, name)];
}

void create_script(const char* path, const char* name)
{
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


//
// ---- materials ----
// 

int get_material_idx(char* name)
{
	return shget(materials, name);
}

material* add_material(shader s, texture dif_tex, texture spec_tex, bee_bool is_transparent, f32 shininess, vec2 tile, vec3 tint, const char* name)
{
	material mat = make_material_tint(s, dif_tex, spec_tex, is_transparent, shininess, tile, tint, name);
	
	// make a persistent copy of the passed name
	char* name_cpy = calloc(strlen(name), sizeof(char));
	assert(name_cpy != NULL);
	strcpy(name_cpy, name);

	shput(materials, name_cpy, materials_data_len);
	materials_len++;
	arrput(materials_data, mat);
	materials_data_len++;

	return &materials_data[shget(materials, name)];
}

material* get_material(char* name)
{
	// @TODO: add security check that the texture doesn't exist at all
	//		  for example make the 0th texture allways be all pink etc.

	// retrieve mesh from mesh_data array
	return &materials_data[shget(materials, name)];
}

material* get_all_materials(int* materials_len)
{
	*materials_len = materials_data_len;
	return materials_data;
}


//
// ---- shaders ----
// 

int get_shader_idx(char* name)
{
	return shget(shaders, name);
}

shader add_shader(const char* vert_name, const char* frag_name, const char* name)
{
	// key for the path is the index of the file in the hashmap
	int path_idx = shgeti(vert_files, vert_name);
	char* vert_path = hmget(vert_files_paths, path_idx);
	path_idx = shgeti(frag_files, frag_name);
	char* frag_path = hmget(frag_files_paths, path_idx);
	shader s = create_shader_from_file(vert_path, frag_path, name);
	s.vert_name = vert_name;
	s.frag_name = frag_name;

	// make a persistent copy of the passed name
	char* name_cpy = calloc(strlen(name), sizeof(char));
	assert(name_cpy != NULL);
	strcpy(name_cpy, name);

	shput(shaders, name_cpy, shaders_data_len);
	shaders_len++;
	arrput(shaders_data, s);
	shaders_data_len++;

	return shaders_data[shget(shaders, name)];
}

shader get_shader(char* name)
{
	// @TODO: add security check that the texture doesn't exist at all
	//		  for example make the 0th texture allways be all pink etc.

	// retrieve mesh from mesh_data array
	return shaders_data[shget(shaders, name)];
}

shader* get_all_shaders(int* shaders_len)
{
	*shaders_len = shaders_data_len;
	return shaders_data;
}

void log_vert_file(const char* path, const char* name)
{
	// make a persistent copy of the passed name
	char* name_cpy = calloc(strlen(name) + 1, sizeof(char));
	assert(name_cpy != NULL);
	strcpy(name_cpy, name);
	// printf("texture name copy: \"%s\"\n", name_cpy);

	// put that copy of the name into the hashmap as the key 
	// to a value of 9999 as that indicates the asset hasnt been loaded yet
	shput(vert_files, name_cpy, 9999);
	vert_files_len++;

	// make a persistent copy of the passed path
	char* path_cpy = calloc(strlen(path) + 1, sizeof(char));
	assert(path_cpy != NULL);
	strcpy(path_cpy, path);

	// printf("vert_file path: \"%s\"\n", path);

	// put that copy of the path into the hashmap
	int i = shgeti(vert_files, name);
	hmput(vert_files_paths, i, path_cpy);
	vert_files_paths_len++;

	// not freeing name_cpy and path_cpy as they need to be used in the future
}

void log_frag_file(const char* path, const char* name)
{
	// make a persistent copy of the passed name
	char* name_cpy = calloc(strlen(name) + 1, sizeof(char));
	assert(name_cpy != NULL);
	strcpy(name_cpy, name);
	// printf("texture name copy: \"%s\"\n", name_cpy);

	// put that copy of the name into the hashmap as the key 
	// to a value of 9999 as that indicates the asset hasnt been loaded yet
	shput(frag_files, name_cpy, 9999);
	frag_files_len++;

	// make a persistent copy of the passed path
	char* path_cpy = calloc(strlen(path) + 1, sizeof(char));
	assert(path_cpy != NULL);
	strcpy(path_cpy, path);

	// put that copy of the path into the hashmap
	int i = shgeti(frag_files, name);
	hmput(frag_files_paths, i, path_cpy);
	frag_files_paths_len++;

	// not freeing name_cpy and path_cpy as they need to be used in the future
}
