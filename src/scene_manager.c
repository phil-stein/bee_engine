#include "scene_manager.h"
#include "asset_manager.h"
#include "file_handler.h"
#include "serialization.h"
#include "renderer.h"


char* active_scene = "no_name.scene";

void load_scene(char* name)
{
	renderer_clear_scene();

	char* path = get_scene_path(name);
	assert(path != NULL);
	int len = 0;
	char* txt  = read_text_file_len(path, &len);
	printf("read scene length: %d\n", len);

	int offset = 0;
	rtn_code rtn = BEE_OK;
	scene s = deserialize_scene(txt, &offset, &rtn);

	if (rtn == BEE_ERROR)
	{
		printf("[ERROR] failed to get scene, abort loading\n");
		return;
	}

	for (int i = 0; i < s.entities_len; ++i)
	{
		add_entity_direct(s.entities[i]);
	}

	active_scene = name;
}

void save_scene(char* name)
{
	// write to file

	char* path = get_asset_dir();
	strcat(path, "\\scenes\\");
	strcat(path, name);
	// strcat(path, "test02.scene");

	scene s;
	s.entities = get_entites();
	get_entity_len(&s.entities_len);


	char buffer[10000];
	int offset = 0;
	serialize_scene(buffer, &offset, &s);

	printf("serialized scene length -> %d\n", offset);

	write_text_file(path, buffer, offset);
}

char* get_active_scene_name()
{
	return active_scene;
}