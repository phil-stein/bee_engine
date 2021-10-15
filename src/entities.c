#include "entities.h"

#include "stb/stb_ds.h" // STB_DS_IMPLEMENTATION defined in renderer.c

#include "gravity_interface.h"
#include "asset_manager.h"
#include "editor_ui.h"
#include "renderer.h"


//
// move all entity related funtions here
// 


// entities
struct { int key; entity value; }*entities = NULL;
// entity* entities = NULL;
int  entities_len = 0;
int* entity_ids = NULL;
int  entity_ids_len = 0;


void entities_init()
{
	// set default return if key doesn't exist
	entity e;
	e.has_trans = 0; e.has_model = 0; e.has_cam = 0;  e.has_light = 0;
	e.name = "x"; e.id = -1; e.id_idx = -1;
	e.scripts_len = 0; e.children_len = 0; e.parent = 9999;
	hmdefault(entities, e);
	printf(" -> entities_init() called\n");
	entity* ent = get_entity(112);
	printf("[ent].id: %d\n", ent->id);
}

void entities_update()
{
	// entities ---------------------------------------------------------------

	for (int i = 0; i < entity_ids_len; ++i)
	{
		update_entity(get_entity(entity_ids[i]));
	}
	gravity_check_for_pending_actions(); // check if one of the run scripts requested to change scene / etc

	// ------------------------------------------------------------------------
}

void entities_cleanup()
{
	for (int i = 0; i < entities_len; ++i)
	{
		free_entity(&entities[i]);
	}

	hmfree(entities);
	arrfree(entity_ids);
}

void entities_clear_scene()
{
	hmfree(entities);
	entities = NULL;
	entities_len = 0;

	arrfree(entity_ids);
	entity_ids = NULL;
	entity_ids_len = 0;


	// set default return if key doesn't exist
	entity e;
	e.has_trans = 0; e.has_model = 0; e.has_cam = 0;  e.has_light = 0;
	e.name = "x"; e.id = -1; e.id_idx = -1;
	e.scripts_len = 0; e.children_len = 0; e.parent = 9999;
	hmdefault(entities, e);
}

// add an entity
int add_entity(vec3 pos, vec3 rot, vec3 scale, mesh* _mesh, material* _material, camera* _cam, light* _light, char* name)
{
	return add_entity_direct(make_entity(pos, rot, scale, _mesh, _material, _cam, _light, name));
}
int add_entity_direct(entity e)
{
	// check id available
	int idx = entities_len;
	bee_bool id_taken = BEE_TRUE;
	int debug = 0;
	while (id_taken)
	{
		entity* ent = get_entity(idx);
		id_taken = ent->id == -1 ? BEE_FALSE : BEE_TRUE; // default ent returned when id not found
		if (id_taken) { idx++; } // printf("id taken now: %d, ent->id: %d\n", idx, ent->id);

		debug++;
		if (debug > 100) { assert(0); }
	}
	add_entity_direct_id(e, idx);
}
int add_entity_direct_id(entity e, int id)
{

	if (e.has_model && e._material != NULL && e._material->is_transparent)
	{
		(*get_transparent_ids_len())++;
		arrput(*get_transparent_ids_ptr(), entities_len);
	}

	if (e.has_light)
	{
		switch (e._light.type)
		{
		case DIR_LIGHT:
			arrput(*get_dir_light_ids_ptr(), entities_len);
			e._light.id = *get_dir_light_ids_len();
			(*get_dir_light_ids_len())++;
			break;
		case POINT_LIGHT:
			arrput(*get_point_light_ids_ptr(), entities_len);
			e._light.id = *get_point_light_ids_len();
			(*get_point_light_ids_len())++;
			break;
		case SPOT_LIGHT:
			arrput(*get_spot_light_ids_ptr(), entities_len);
			e._light.id = *get_spot_light_ids_len();
			(*get_spot_light_ids_len())++;
			break;
		}
	}

	if (e.has_cam)
	{
		if (get_camera_ent_id() != -1) // there already exists a camera
		{
			set_error_popup(GENERAL_ERROR, "Entity with Camera added but one already was present.");
			printf("Entity with Camera added but one already was present.\n");
			return -1;
		}
		set_camera_ent_id(entities_len);
	}

	assert(id >= 0);
	e.id = id < 0 ? entities_len : id;
	e.id_idx = entity_ids_len;

	// hmput(entities, entities_len, e);
	hmput(entities, e.id, e);
	// printf("added entity: \"%s\", idx: %d\n", e.name, entities_len);
	// arrput(entity_ids, entities_len);
	arrput(entity_ids, e.id);
	entities_len++;
	entity_ids_len++;

	// sets dir & front vec in light & cam
	entity_set_rot(id, e.rot);

	return entities_len - 1;
}
int duplicate_entity(int id)
{
	if (id == get_camera_ent_id())
	{
		set_error_popup(GENERAL_ERROR, "Cannot duplicate Camera Entity as only one Camera is allowed to exist.");
		printf("Cannot duplicate Camera Entity as only one Camera is allowed to exist.\n");
		return -1;
	}
	entity e;
	entity* original = get_entity(id);
	// memcpy(&e, original, sizeof(entity));
	e.id = -1;
	e.id_idx = -1;
	char* name = malloc(strlen(original->name) + 2 + 1);
	sprintf_s(name, strlen(original->name) + 2 + 1, "%s02", original->name);
	e.name = name;

	e.has_trans = original->has_trans;
	if (e.has_trans)
	{
		glm_vec3_copy(original->pos, e.pos);
		glm_vec3_copy(original->rot, e.rot);
		glm_vec3_copy(original->scale, e.scale);
	}
	e.rotate_global = original->rotate_global;
	e.has_model = original->has_model;
	if (e.has_model)
	{
		e._mesh = original->_mesh;
		e._material = original->_material;
	}
	e.has_cam = original->has_cam;
	if (e.has_cam)
	{
		e._camera = original->_camera;
	}
	e.has_light = original->has_light;
	if (e.has_light)
	{
		e._light = original->_light;
	}

	for (int i = 0; i < e.children_len; ++i)
	{
		e.children[i] = 0;
	}
	e.children_len = 0;
	e.parent = 9999;

	e.scripts = NULL;
	for (int i = 0; i < original->scripts_len; ++i)
	{
		arrput(e.scripts, original->scripts[i]);
		e.scripts_len++;
	}

	return add_entity_direct(e);
}

void entity_switch_light_type(int entity_id, light_type new_type)
{
	entity* ent = get_entity(entity_id);
	if (!ent->has_light) { return; }
	// remove from old type categorisation
	switch (ent->_light.type)
	{
	case DIR_LIGHT:
		arrdel((*get_dir_light_ids_ptr()), ent->_light.id);
		(*get_dir_light_ids_len())--;
		break;
	case POINT_LIGHT:
		arrdel((*get_point_light_ids_ptr()), ent->_light.id);
		(*get_point_light_ids_len())--;
		break;
	case SPOT_LIGHT:
		arrdel((*get_spot_light_ids_ptr()), ent->_light.id);
		(*get_spot_light_ids_len())--;
		break;
	}

	// add to new type categorisation
	switch (new_type)
	{
	case DIR_LIGHT:
		arrput((*get_dir_light_ids_ptr()), entity_id);
		(*get_dir_light_ids_len())++;
		create_framebuffer_shadowmap(&ent->_light.shadow_map, &ent->_light.shadow_fbo, 2048, 2048);
		break;
	case POINT_LIGHT:
		arrput((*get_point_light_ids_ptr()), entity_id);
		(*get_point_light_ids_len())++;
		break;
	case SPOT_LIGHT:
		arrput((*get_spot_light_ids_ptr()), entity_id);
		(*get_spot_light_ids_len())++;
		break;
	}

	ent->_light.type = new_type;
}

void entity_add_script(int entity_index, const char* name)
{
	entity* ent = get_entity(entity_index);
	gravity_script* script = get_script(name);
#ifdef EDITOR_ACT
	script->active = BEE_FALSE;
#endif
	arrput(ent->scripts, script);
	ent->scripts_len++;

}
void entity_remove_script(int entity_index, int script_index)
{
	entity* ent = &hmget(entities, entity_index);
	free_script(ent->scripts[script_index]);
	arrdel(ent->scripts, script_index);
	ent->scripts_len--;
}

void entity_set_rot(int entity_id, vec3 rot)
{
	entity* e = get_entity(entity_id);
	glm_vec3_copy(rot, e->rot);

	if (e->has_light && e->_light.type == DIR_LIGHT)
	{
		// vec3 dir = { rot[0] / 360, rot[1] / 360, rot[2] / 360 };
		vec3 dir = { rot[0] / 180, rot[1] / 180, rot[2] / 180 };
		glm_vec3_normalize(dir);
		glm_vec3_copy(dir, e->_light.direction);
	}
	if (e->has_cam)
	{
		// vec3 dir = { rot[0] / 360, rot[1] / 360, rot[2] / 360 };

		// vec3 dir = { rot[0] / 180, rot[1] / 180, rot[2] / 180 };
		// glm_vec3_normalize(dir);
		// glm_vec3_copy(dir, e->_camera.front);
	}
}
void entity_set_dir_vec(int entity_id, vec3 dir)
{
	entity* e = get_entity(entity_id);
	// vec3 dir_val = { dir[0] * 360, dir[1] * 360, dir[2] * 360 };
	vec3 dir_val = GLM_VEC3_ZERO_INIT;
	glm_vec3_copy(dir, dir_val);
	// vec3 mul = { 360, 360, 360 };
	// vec3 mul = { 180, 180, 180 };
	vec3 mul = { 360 / 3.0f, 360 / 3.0f, 360 / 3.0f };
	glm_vec3_normalize(dir_val);
	glm_vec3_mul(dir_val, mul, dir_val);
	glm_vec3_copy(dir_val, e->rot);

	if (e->has_light)
	{
		glm_vec3_copy(dir, e->_light.direction);
	}
	if (e->has_cam)
	{
		glm_vec3_copy(dir, e->_camera.front);
	}
}



void entity_set_parent(int child, int parent)
{
	if (child == parent) { return; }

	entity* parent_ent = get_entity(parent);
	entity* child_ent = get_entity(child);
	if (!parent_ent->has_trans || !child_ent->has_trans)
	{
		printf("assign child: \"%s\" to entity: \"%s\" failed\n", child_ent->name, parent_ent->name);
		return;
	}


	int* parent_ptr = &child_ent->parent;
	if (child_ent->parent != 9999)
	{
		entity_remove_child(child_ent->parent, child);
	}

	// keep the relative transform before being a child
	vec3 pos = { 0.0f, 0.0f, 0.0f };
	vec3 rot = { 0.0f, 0.0f, 0.0f };
	vec3 scale = { 0.0f, 0.0f, 0.0f };
	get_entity_global_transform(parent, pos, rot, scale);
	glm_vec3_sub(child_ent->pos, parent_ent->pos, child_ent->pos);
	// rotation
	glm_vec3_div(child_ent->scale, parent_ent->scale, child_ent->scale);

	child_ent->parent = parent;

	arrput(parent_ent->children, child);
	parent_ent->children_len++;
}
void entity_remove_child(int parent, int child)
{
	if (child == parent) { return; }


	vec3 pos = { 0.0f, 0.0f, 0.0f };
	vec3 rot = { 0.0f, 0.0f, 0.0f };
	vec3 scale = { 0.0f, 0.0f, 0.0f };
	get_entity_global_transform(child, pos, rot, scale);
	entity* child_ent = (get_entity(child));
	printf("removed child: \"%s\" of parent: \"%s\"\n", child_ent->name, (get_entity(parent))->name);
	child_ent->parent = 9999;
	// set the childs transform to be the one it was while it still was a child
	glm_vec3_copy(pos, child_ent->pos);
	glm_vec3_copy(rot, child_ent->rot);
	glm_vec3_copy(scale, child_ent->scale);


	// remove child from children array
	for (int i = 0; i < (get_entity(parent))->children_len; ++i)
	{
		if ((get_entity(parent))->children[i] == child)
		{
			arrdel((get_entity(parent))->children, i);
			(get_entity(parent))->children_len--;
			return;
		}
	}
}

void entity_remove(int entity_idx)
{
	entity* ent = get_entity(entity_idx);
	if (ent->has_light)
	{
		switch (ent->_light.type)
		{
		case DIR_LIGHT:
			for (int i = 0; i < (*get_dir_light_ids_len()); ++i)
			{
				if ((*get_dir_light_ids_ptr())[i] == entity_idx)
				{
					arrdel((*get_dir_light_ids_ptr()), i);
					(*get_dir_light_ids_len())--;
					break;
				}
			}
		case POINT_LIGHT:
			for (int i = 0; i < (*get_point_light_ids_len()); ++i)
			{
				if ((*get_point_light_ids_ptr())[i] == entity_idx)
				{
					arrdel((*get_point_light_ids_ptr()), i);
					(*get_point_light_ids_len())--;
					break;
				}
			}
		case SPOT_LIGHT:
			for (int i = 0; i < (*get_spot_light_ids_len()); ++i)
			{
				if ((*get_spot_light_ids_ptr())[i] == entity_idx)
				{
					arrdel((*get_spot_light_ids_ptr()), i);
					(*get_spot_light_ids_len())--;
					break;
				}
			}
		}
	}

	if (ent->has_cam && entity_idx == get_camera_ent_id()) { set_camera_ent_id(-1); }

	// unparent all children
	for (int i = 0; i < ent->children_len; ++i)
	{
		entity_remove_child(entity_idx, ent->children[i]);
	}
	assert(ent->children_len == 0);

	if (ent->parent != 9999)
	{
		entity_remove_child(ent->parent, entity_idx);
	}

	int id_idx = ent->id_idx;
	hmdel(entities, entity_idx);
	entities_len--;
	arrdel(entity_ids, id_idx);
	entity_ids_len--;

	// update the id_idx
	int ids_len = 0;
	int* ids = get_entity_ids(&ids_len);
	for (int i = id_idx; i < ids_len; ++i)
	{
		get_entity(ids[i])->id_idx = i;
	}
}

void get_entity_len(int* _entities_len)
{
	*_entities_len = entities_len;
}
int* get_entity_ids(int* len)
{
	*len = entity_ids_len;
	return entity_ids;
}
entity* get_entity(int id)
{
	return &hmget(entities, id);
}
entity* get_cam_entity()
{
	return get_entity(get_camera_ent_id());
}
int get_entity_id_by_name(char* name)
{
	for (int i = 0; i < entities_len; ++i)
	{
		if (strcmp(get_entity(entity_ids[i])->name, name) == 0)
		{
			return entity_ids[i];
		}
	}
	assert(0); // no entity with given name
	return 9999;
}
