#include "asset_manager.h"

#include <direct.h>
// #include <sys/types.h>

#include "stb/stb_ds.h" // STB_DS_IMPLEMENTATION defined in renderer.c
#include "dirent/dirent.h"

#include "file_handler.h"

// ---- vars ----
// hashmaps using stb_ds.h
// prob. should replace these with something faster sometime [I mean maybe but it's actually pretty fast]
struct { char* key;  int  value;  }* textures  = NULL;
struct { int key;	 char* value; }*texture_paths = NULL;
int tex_len = 0;
texture* tex = NULL;

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

	shfree(materials);
	shfree(meshes);

	// arrfree(tex);
}

texture get_texture(const char* name)
{
	int i = shget(textures, name);
	if (i == NULL)
	{
		fprintf(stderr, "");
	}
	else if (i == 9999)
	{
		create_texture(name);
	}
	return tex[shget(textures, name)];
}

void log_texture(const char* path, const char* name)
{
	shput(textures, name, 9999);
	int i = shgeti(textures, name);
	hmput(texture_paths, i, path);
	printf("texture path: %s\n", path);
}

void create_texture(const char* name)
{
	assert(1 == 0); // not yet working
	const char* path = NULL; // log this and retrieve it here
	texture t = texture_create_from_path(path, name, BEE_FALSE);

	shput(textures, name, tex_len);
	arrput(tex, t);
	tex_len++;
}