#include "scene_manager.h"
#include "asset_manager.h"
#include "file_handler.h"
#include "serialization.h"
#include "stb/stb_ds.h"
#include "renderer.h"

#define NAME_SIZE 25

char scene_state_buffer[10000];
char scene_name_before_state_change[NAME_SIZE];

char active_scene[NAME_SIZE] = "no_name";

char* get_active_scene_name()
{
	return active_scene;
}

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
		add_entity_direct_id(s.entities[i], s.entities[i].id);
	}

	strcpy_s(active_scene, NAME_SIZE, name);
}

void save_scene(char* name)
{
	char* path = get_asset_dir();
	strcat(path, "\\scenes\\");
	strcat(path, name);

	scene s;
	s.entities = NULL;
	int* entity_ids = get_entity_ids(&s.entities_len);
	for (int i = 0; i < s.entities_len; ++i)
	{
		arrput(s.entities, *get_entity(entity_ids[i]));
	}

	char buffer[10000];
	int offset = 0;
	serialize_scene(buffer, &offset, &s);

	printf("serialized scene length -> %d\n", offset);

	write_text_file(path, buffer, offset);

	strcpy_s(active_scene, NAME_SIZE, name);
}

void save_scene_state()
{
	scene s;
	s.entities = NULL;
	int* entity_ids = get_entity_ids(&s.entities_len);
	for (int i = 0; i < s.entities_len; ++i)
	{
		arrput(s.entities, *get_entity(entity_ids[i]));
	}


	int offset = 0;
	serialize_scene(scene_state_buffer, &offset, &s);

	strcpy_s(scene_name_before_state_change, NAME_SIZE, active_scene);

	printf("serialized scene length -> %d\n", offset);
}

void load_scene_state()
{
	renderer_clear_scene();

	int offset = 0;
	rtn_code rtn = BEE_OK;
	scene s = deserialize_scene(scene_state_buffer, &offset, &rtn);

	if (rtn == BEE_ERROR)
	{
		printf("[ERROR] failed to get scene, abort loading\n");
		return;
	}

	for (int i = 0; i < s.entities_len; ++i)
	{
		add_entity_direct_id(s.entities[i], s.entities[i].id);
	}

	strcpy_s(active_scene, NAME_SIZE, scene_name_before_state_change);
}

