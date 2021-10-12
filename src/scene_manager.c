#include "scene_manager.h"
#include "asset_manager.h"
#include "file_handler.h"
#include "serialization.h"
#include "stb/stb_ds.h"
#include "entities.h"
#include "renderer.h"

#define NAME_SIZE 25

#ifdef EDITOR_ACT
char scene_state_buffer[10000];
char scene_name_before_state_change[NAME_SIZE];
#endif

char active_scene[NAME_SIZE] = "no_name";

char* get_scene_dir_path()
{
	char* ptr = get_asset_dir();
	static char path[100];
	strcpy_s(path, 100, ptr);
	strcat_s(path, 100, "\\scenes\\");
	return path;
}

char* get_active_scene_name()
{
	return active_scene;
}


void load_scene(const char* name)
{
	clear_scene();

	char* path = get_scene_dir_path(name);
	assert(path != NULL);
	strcat(path, name);
	int len = 0;
	char* txt  = read_text_file_len(path, &len);
	printf("read scene length: %d\n", len);

	int offset = 0;
	rtn_code rtn = BEE_OK;
	scene s = deserialize_scene(txt, &offset, &rtn);
	free(txt);

	if (rtn == BEE_ERROR)
	{
		printf("[ERROR] failed to get scene, abort loading\n");
		return;
	}

	// settings
	*get_exposure() = s.exposure;
	renderer_set(RENDER_MSAA, s.use_msaa);
	set_bg_color(s.bg_color);													

	// entities
	for (int i = 0; i < s.entities_len; ++i)
	{
		add_entity_direct_id(s.entities[i], s.entities[i].id);
	}

#ifdef EDITOR_ACT
	if (get_camera_ent_id() == -1 && get_gamestate()) // no camera entity
	{
		set_gamestate(BEE_FALSE, BEE_TRUE);
		char buf[40 + NAME_SIZE]; 
		sprintf_s(buf, 40 + NAME_SIZE, "Scene loaded has no camera. Scene: \"%s\"", name);
		set_error_popup(GENERAL_ERROR, buf); 
	}
#else
	if (get_camera_ent_id() == -1)
	{
		assert(0); // scene without camera loaded
	}
#endif

	strcpy_s(active_scene, NAME_SIZE, name);

}

void save_scene(const char* name)
{
	char* path = get_scene_dir_path();
	strcat(path, name);

	scene s;
	// settings
	s. exposure = *get_exposure();			
	s.use_msaa = *renderer_get(RENDER_MSAA);
	vec3 bg; get_bg_color(bg);								
	glm_vec3_copy(bg, s.bg_color);

	// entities
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

	arrfree(s.entities);

	// check with asset managers
	log_scene(path, name);
}

void clear_scene()
{
	entities_clear_scene();
	renderer_clear_scene();
}

void add_empty_scene(const char* name)
{
	char* path = get_scene_dir_path();
	strcat(path, name);

	scene s;
	s.entities = NULL;
	s.entities_len = 0;

	char buffer[1000];
	int offset = 0;
	serialize_scene(buffer, &offset, &s);

	printf("empty scene length -> %d\n", offset);

	write_text_file(path, buffer, offset);

	strcpy_s(active_scene, NAME_SIZE, name);

	log_scene(path, name);
}
void add_default_scene(const char* name)
{
	char* path = get_scene_dir_path();
	strcat(path, name);

	scene s;
	s.entities = NULL;
	s.entities_len = 0;

	shader* shader_default = add_shader("basic.vert", "blinn_phong.frag", "SHADER_default");

	shader* shader_cel = add_shader("basic.vert", "cel_shading.frag", "SHADER_cel");

	vec2 tile = { 1.0f, 1.0f };
	vec3 tint = { 1.0f, 1.0f, 1.0f };
	texture blank_tex = get_texture("blank.png");
	material* mat_blank = add_material(shader_default, blank_tex, blank_tex, BEE_FALSE, 1.0f, tile, tint, BEE_FALSE, "MAT_blank", BEE_TRUE);
	material* mat_cel = add_material(shader_cel, blank_tex, blank_tex, BEE_FALSE, 1.0f, tile, tint, BEE_FALSE, "MAT_cel", BEE_TRUE);
	
	vec3 zero = { 0, 0, 0 };
	vec3 one  = { 1, 1, 1 };

	// in-game-camera
	camera cam = make_camera(40.0f, 0.1f, 100.0f);
	cam.front[0] =  0;
	cam.front[1] = -1;
	cam.front[2] =  0;
	vec3 cam_pos = { 0.0f, 2.0f,  3.5f };
	mesh* m_camera = get_mesh("camera.obj");	
	arrput(s.entities, make_entity(cam_pos, zero, one, NULL, NULL, &cam, NULL, "camera"));
	s.entities_len++;

	arrput(s.entities, make_entity(zero, zero, one, get_mesh("cube.obj"), get_material("MAT_blank"), NULL, NULL, "cube"));
	s.entities_len++;

	vec3 ground_pos   = { 0.0f, -0.5f,  0.0f };
	vec3 ground_scale = { 10.0f, 1.0f,  10.0f };
	arrput(s.entities, make_entity(ground_pos, zero, ground_scale, get_mesh("cube.obj"), get_material("MAT_blank"), NULL, NULL, "ground"));
	s.entities_len++;

	vec3 ambient = { 0.0f, 0.0f, 0.0f };
	vec3 diffuse01 = { 1.0f, 1.0f, 1.0f };
	vec3 diffuse02 = { 0.9f, 0.25f, 0.85f }; //rgb(231, 65, 218)
	vec3 specular = { 1.0f, 1.0f, 1.0f };
	vec3 direction = { -0.2f, -1.0f, -0.3f };
	vec3 direction02 = { -0.2f, -0.8f, -0.5f };
	light dir_light = make_dir_light(ambient, diffuse01, specular, direction);
	light point_light = make_point_light(ambient, diffuse02, specular, 1.0f, 0.14f, 0.13f); // 0.09f, 0.032f);


	vec3 pos_light01 = { 0.0f, 2.0f, 0.0f };
	vec3 pos_light02 = { 0.75f, 0.0f, 0.0f };
	arrput(s.entities, make_entity(pos_light01, zero, one, NULL, NULL, NULL, &dir_light, "dir_light"));
	s.entities_len++;
	
	arrput(s.entities, make_entity(pos_light02, zero, one, NULL, NULL, NULL, &point_light, "point_light"));
	s.entities_len++;

	char buffer[2000];
	int offset = 0;
	serialize_scene(buffer, &offset, &s);

	printf("default scene length -> %d\n", offset);

	write_text_file(path, buffer, offset);

	strcpy_s(active_scene, NAME_SIZE, name);

	arrfree(s.entities);

	log_scene(path, name);
}


#ifdef EDITOR_ACT
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
	clear_scene();

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
#endif
