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

	// printf("\n-------------\n");
	search_dir(dir_path);
	// printf("\n-------------\n");

	//
	// !!! the values dont get put into the array or 9999 is overflow !!!
	//

	// debug print the contents of textures
	for (int i = 0; i < textures_len; ++i)
	{
		printf("%d - texture_id: %d\n", i, textures[i]);
		// if (shget(textures, i) != 9999)
		// {
		// 	int idx = shget(textures, i);
		// 	printf("-> \"%s\"", hmget(texture_paths, idx));
		// }
	}

}

void search_dir(const char* dir_path)
{
	char path[250];
	struct dirent* dp;
	DIR* dir = opendir(dir_path);
	// unable to open directory stream
	if (!dir) { return; }

	while ((dp = readdir(dir)) != NULL)
	{
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
				strcat(t_path, dp->d_name);
				
				// printf("dir_path:     '%s'\n", dir_path);
				// printf("dir_path_cpy: '%s'\n", dir_path_cpy);
				// printf("path:         '%s'\n---------------\n", t_path);

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
	// free the allocated memory
	shfree(textures);
	hmfree(texture_paths);
	arrfree(tex);

	shfree(materials);
	shfree(meshes);
}

texture get_texture(const char* name)
{
	printf("requested texture: \"%s\"\n", name);
	int i = shget(textures, name);

	if (0 == 1) // check if texture exits
	{
		fprintf(stderr, "ERROR: Requested Texture hasn't been logged.\n");
	}
	else if (i == 9999 || i > tex_len) // 0 == 0
	{
		create_texture(name);
	}

	printf("id requested texture: %d\n", i);

	printf("arrlen tex: %d, tex_len: %d\n", arrlen(tex), tex_len);
	texture result = tex[shget(textures, name)];

	assert(&result);
	
	return result;
}

void log_texture(const char* path, const char* name)
{
	shput(textures, name, 9999);
	textures_len++;
	int i = shgeti(textures, name);
	hmput(texture_paths, i, path);
	texture_paths_len++;
	printf("texture path: \"%s\"; texture name: \"%s\"; texture id: %d\n", path, name, i);
}

void create_texture(const char* name)
{
	int path_idx = (int)shgeti(textures, name);
	const char* path = hmget(texture_paths, path_idx); // log this and retrieve it here
	printf("-> create texture path: \"%s\"; path index: %d\n", path, path_idx);
	texture t = texture_create_from_path(path, name, BEE_FALSE);

	shput(textures, name, tex_len);
	arrput(tex, t);
	tex_len++;
}