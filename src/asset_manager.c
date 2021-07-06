#include "asset_manager.h"

#include <direct.h>
// #include <sys/types.h>

#include "stb/stb_ds.h" // STB_DS_IMPLEMENTATION defined in renderer.c
#include "dirent/dirent.h"

#include "file_handler.h"

// ---- vars ----
// hashmaps & dyn.-arrays using stb_ds.h
// prob. should replace these with something faster sometime [I mean maybe but it's actually pretty fast]

// value: index of texture in 'tex', key: name of texture 
struct { char* key;  int   value; }* textures	   = NULL;
int textures_len = 0;

// value: path to image, key: texture-index in 'textures'
struct { int   key;	 char* value; }* texture_paths = NULL; // turn this into an array, god damn ahhhh
int texture_paths_len = 0;

// array of holding textures
texture* tex = NULL;
int tex_len = 0;

struct { char* key;  material value; }* materials = NULL;
struct { char* key;  mesh     value; }* meshes    = NULL;
// light* lights; // seems weird idk why



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

	// printf("'crate01_dif.png' texture id: '%d'\n", shget(textures, "crate01_dif.png"));
	// printf("'crate01_spec.png' texture id: '%d'\n", shget(textures, "crate01_spec.png"));
	// printf("'blank.png' texture id: '%d'\n", shget(textures, "blank.png"));
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
				
				// printf("dir_path:     '%s'\n", dir_path);
				// printf("dir_path_cpy: '%s'\n", dir_path_cpy);
				// printf("path:         '%s'\n---------------\n", t_path);

				// log the texture with the created path and the file name 
				log_texture(t_path, dp->d_name);
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
	arrfree(tex);

	shfree(materials);
	shfree(meshes);

}

texture* get_all_textures(int* textures_len)
{
	*textures_len = tex_len;
	return tex;
}

texture get_texture(const char* name)
{
	// get the index to the tex array from the hashmap
	int i = shget(textures, name);
	// printf("requested texture: \"%s\", id: \"%d\"\n", name, i);

	// @TODO: add security check that the texture doesn't exist at all
	//		  for example make the 0th texture allways be all pink etc.

	// check if the texture hasn't been loaded yet 
	if (i == 9999 || i > tex_len) // 0 == 0
	{
		// printf(" ---- crating texture ----\n");
		create_texture(name);
	}

	// printf("id requested texture: %d\n", i);

	// printf("arrlen tex: %d, tex_len: %d\n", arrlen(tex), tex_len);

	// retrieve texture from tex array
	texture result = tex[shget(textures, name)];
	
	return result;
}

void log_texture(const char* path, const char* name)
{
	// make a persistent copy of the passed name
	int len = strlen(name);
	char* name_cpy = calloc(len, sizeof(char));
	assert(name_cpy != NULL);
	strcpy(name_cpy, name);
	// printf("texture name copy: \"%s\"\n", name_cpy);
	
	// put that copy of the name into the hashmap as the key 
	// to a value of 9999 as that indicates the asset hasnt been loaded yet
	shput(textures, name_cpy, 9999);
	textures_len++;
	
	// make a persistent copy of the passed path
	len = strlen(path);
	char* path_cpy = calloc(len, sizeof(char));
	assert(path_cpy != NULL);
	strcpy(path_cpy, path);
	// printf("texture path copy: \"%s\"\n", path_cpy);
	
	// put that copy of the path into the hashmap
	int i = shgeti(textures, name);
	hmput(texture_paths, i, path_cpy);
	texture_paths_len++;
	// printf("texture path: \"%s\"; texture name: \"%s\"; texture id: %d\n", path, name, i);
	
	// not freeing name_cpy and path_cpy as they need to be used in the future
}

void create_texture(const char* name)
{
	// key for the path is the index of the texture in the hashmap
	int path_idx = shgeti(textures, name);
	char* path = hmget(texture_paths, path_idx);
	// printf("-> create texture path: \"%s\"; path index: %d\n", path, path_idx);
	
	texture t = texture_create_from_path(path, name, BEE_FALSE);

	// put texture index in tex array into the value of the hashmap with the texture name as key 
	// and put the created texture into the tex array
	shput(textures, name, tex_len);
	arrput(tex, t);
	tex_len++;
}