#include "serialization.h"
#include "asset_manager.h"
#include "stb/stb_ds.h"

#include "renderer.h" // tmp
#include "scene_manager.h" // tmp

void test_serialization()
{
	int int_val = -123456789;
	f32 f32_val = 1234.56789f;
	u32 u32_val = 123456789;
	bee_bool bool_val = BEE_TRUE;
	vec3 vec_val = { 0.1f, 11.1f, 1234 };

	char buffer[1024];
	int offset_ser = 0;
	int offset_des = 0;
	serialize_int(&buffer, &offset_ser, int_val);
	deserialize_int(buffer, &offset_des);

	serialize_float(buffer, &offset_ser, f32_val);
	deserialize_float(buffer, &offset_des);

	serialize_u32(buffer, &offset_ser, u32_val);
	deserialize_u32(buffer, &offset_des);

	serialize_str(buffer, &offset_ser, "hi, there :)");
	char* str = deserialize_str(buffer, &offset_des);
	free(str);

	serialize_enum(buffer, &offset_ser, bool_val);
	bee_bool bool_rtn = deserialize_enum(buffer, &offset_des);
	printf("bool: %d\n", bool_rtn);

	serialize_vec3(buffer, &offset_ser, vec_val);
	vec3 vec_rtn; deserialize_vec3(buffer, &offset_des, vec_rtn);
	printf("vec -  x:%f, y:%f, z:%f\n", vec_rtn[0], vec_rtn[1], vec_rtn[2]);

	texture t = get_texture("crate01_dif.png");
	serialize_texture(buffer, &offset_ser, &t);
	texture result = deserialize_texture(buffer, &offset_des);

	printf("\n------------------------------------\n");
	char buffer_ent[1024];
	int offset_ent_ser = 0;
	int offset_ent_des = 0;
	serialize_entity(buffer_ent, &offset_ent_ser, get_entity(get_entity_id_by_name("robot")));
	entity e = deserialize_entity(buffer_ent, &offset_ent_des);
	add_entity_direct(e); // hell yeah :)
}

// serialize ------------------------------------------------------------
void serialize_scene(char* buffer, int* offset, scene* s)
{
	// serializing assets that arent defined in files

	int shaders_len = 0;
	shader* shaders = get_all_shaders(&shaders_len);

	serialize_int(buffer, offset, shaders_len);

	for (int i = 0; i < shaders_len; ++i)
	{
		serialize_shader(buffer, offset, &shaders[i]);
	}

	int materials_len = 0;
	material* materials = get_all_materials(&materials_len);
	
	serialize_int(buffer, offset, materials_len);

	for (int i = 0; i < materials_len; ++i)
	{
		serialize_material(buffer, offset, &materials[i]);
	}

	serialize_int(buffer, offset, s->entities_len);
	for (int i = 0; i < s->entities_len; ++i)
	{
		serialize_entity(buffer, offset, &s->entities[i]);
	}
	
}

void serialize_entity(char* buffer, int* offset, entity* ent)
{
	serialize_str(buffer, offset, ent->name);

	serialize_enum(buffer, offset, ent->has_trans);
	if (ent->has_trans)
	{
		serialize_vec3(buffer, offset, ent->pos);
		serialize_vec3(buffer, offset, ent->rot);
		serialize_vec3(buffer, offset, ent->scale);
	}
	serialize_enum(buffer, offset, ent->rotate_global);

	serialize_enum(buffer, offset, ent->has_model);
	if (ent->has_model)
	{
		serialize_mesh(buffer, offset, &ent->_mesh);
		serialize_str(buffer, offset, ent->_material->name);
		// serialize_material(buffer, offset, ent->_material);
	}

	serialize_enum(buffer, offset, ent->has_cam);
	serialize_enum(buffer, offset, ent->has_light);
	if (ent->has_cam)
	{
		serialize_camera(buffer, offset, &ent->_camera);
	}
	if (ent->has_light)
	{
		serialize_light(buffer, offset, &ent->_light);
	}

	serialize_int(buffer, offset, ent->scripts_len);
	for (int i = 0; i < ent->scripts_len; ++i)
	{
		serialize_script(buffer, offset, ent->scripts[i]);
	}

	serialize_int(buffer, offset, ent->parent);
	serialize_int(buffer, offset, ent->children_len);
	for (int i = 0; i < ent->children_len; ++i)
	{
		serialize_int(buffer, offset, ent->children[i]);
	}
}

// ---- components ----

void serialize_material(char* buffer, int* offset, material* m)
{
	serialize_shader(buffer, offset, &m->shader);
	serialize_texture(buffer, offset, &m->dif_tex);
	serialize_texture(buffer, offset, &m->spec_tex);

	serialize_enum(buffer, offset, m->is_transparent);
	serialize_float(buffer, offset, m->shininess);
	serialize_vec2(buffer, offset, m->tile);
	serialize_vec3(buffer, offset, m->tint);

	serialize_enum(buffer, offset, m->draw_backfaces);
	serialize_str(buffer, offset, m->name);
}

void serialize_mesh(char* buffer, int* offset, mesh* m)
{
	// serialize_u32(buffer, offset, m->vertices_len);
	// serialize_u32(buffer, offset, m->vertices_elems);
	// serialize_u32(buffer, offset, m->indices_len);
	// serialize_u32(buffer, offset, m->indices_elems);

	// serialize_u32(buffer, offset, m->vao);
	// serialize_u32(buffer, offset, m->vbo);
	// serialize_u32(buffer, offset, m->ebo);

	// serialize_enum(buffer, offset, m->indexed);
	serialize_enum(buffer, offset, m->visible);
	serialize_str(buffer, offset, m->name);
}

void serialize_camera(char* buffer, int* offset, camera* c)
{
	serialize_float(buffer, offset, c->perspective);
	serialize_float(buffer, offset, c->near_plane);
	serialize_float(buffer, offset, c->far_plane);

	serialize_vec3(buffer, offset, c->front);
	serialize_vec3(buffer, offset, c->up);
	serialize_vec3(buffer, offset, c->target);
}

void serialize_light(char* buffer, int* offset, light* l)
{
	serialize_enum(buffer, offset, l->type);

	serialize_vec3(buffer, offset, l->ambient);
	serialize_vec3(buffer, offset, l->diffuse);
	serialize_vec3(buffer, offset, l->specular);

	serialize_vec3(buffer, offset, l->direction);

	serialize_float(buffer, offset, l->constant);
	serialize_float(buffer, offset, l->linear);
	serialize_float(buffer, offset, l->quadratic);

	serialize_float(buffer, offset, l->cut_off);
	serialize_float(buffer, offset, l->outer_cut_off);
}

// ---- internal structs ----

void serialize_shader(char* buffer, int* offset, shader* s)
{
	// serialize_u32(buffer, offset, s->handle);
	serialize_str(buffer, offset, s->vert_name);
	serialize_str(buffer, offset, s->frag_name);
	serialize_str(buffer, offset, s->name);
}

void serialize_texture(char* buffer, int* offset, texture* t)
{
	// serialize_u32(buffer, offset, t->handle);
	// serialize_int(buffer, offset, t->size_x);
	// serialize_int(buffer, offset, t->size_y);
	serialize_str(buffer, offset, t->name);
	// serialize_str(buffer, offset, t->path);
}

void serialize_script(char* buffer, int* offset, gravity_script* s)
{
	// use get_script() to set the rest of the info 
	serialize_str(buffer, offset, s->name);
}


// ---- base types ----

void serialize_int(char* buffer, int* offset, int val)
{
	// write to buffer
	printf("serialize int: %d\n", val);
	sprintf(buffer + *offset, "%d", val);

	// move the offset to after the newly written data
	*offset += sizeof(int);
}

void serialize_float(char* buffer, int* offset, f32 val)
{
	// write to buffer
	printf("serialize f32: %f\n", val);
	sprintf(buffer + *offset, "%f", val);

	// move the offset to after the newly written data
	*offset += sizeof(f32);
}

void serialize_u32(char* buffer, int* offset, u32 val)
{
	// write to buffer
	printf("serialize u32: %d\n", val);
	sprintf(buffer + *offset, "%d", val);
	
	// move the offset to after the newly written data
	*offset += sizeof(u32);
}

void serialize_enum(char* buffer, int* offset, char val)
{
	// write to buffer
	buffer[*offset] = val;

	// move the offset to after the newly written data
	*offset += sizeof(char);
}

void serialize_str(char* buffer, int* offset, char* val)
{
	// write to buffer
	int len = strlen(val);
	serialize_int(buffer, offset, len);

	printf("serialize str: ");;
	for (int i = 0; i < len; ++i)
	{
		buffer[*offset] = val[i];
		printf("%c", buffer[*offset]);
		*offset += sizeof(char);
	}
	printf("\n");
	// move the offset to after the newly written data
}

void serialize_vec2(char* buffer, int* offset, vec2 val)
{
	serialize_float(buffer, offset, val[0]);
	serialize_float(buffer, offset, val[1]);
}

void serialize_vec3(char* buffer, int* offset, vec3 val)
{
	serialize_float(buffer, offset, val[0]);
	serialize_float(buffer, offset, val[1]);
	serialize_float(buffer, offset, val[2]);
}


// deserialize ----------------------------------------------------------

scene deserialize_scene(char* buffer, int* offset)
{

	int shaders_len = deserialize_int(buffer, offset);

	for (int i = 0; i < shaders_len; ++i)
	{
		deserialize_shader(buffer, offset); // automatically adds to the asset-manager
	}

	int materials_len = deserialize_int(buffer, offset);

	for (int i = 0; i < materials_len; ++i)
	{
		deserialize_material(buffer, offset);  // automatically adds to the asset-manager
	}


	scene s;
	s.entities = NULL;
	s.entities_len = deserialize_int(buffer, offset);
	for (int i = 0; i < s.entities_len; ++i)
	{
		arrput(s.entities, deserialize_entity(buffer, offset));
	}
	return s;
}

entity deserialize_entity(char* buffer, int* offset)
{
	entity e;

	e.name = deserialize_str(buffer, offset);
	e.has_trans = deserialize_enum(buffer, offset);
	if (e.has_trans)
	{
		deserialize_vec3(buffer, offset, e.pos);
		deserialize_vec3(buffer, offset, e.rot);
		deserialize_vec3(buffer, offset, e.scale);
	}
	e.rotate_global = deserialize_enum(buffer, offset);

	e.has_model = deserialize_enum(buffer, offset);
	if (e.has_model)
	{
		e._mesh     = *deserialize_mesh(buffer, offset);
		e._material = get_material(deserialize_str(buffer, offset));
	}

	e.has_cam   = deserialize_enum(buffer, offset);
	e.has_light = deserialize_enum(buffer, offset);
	if (e.has_cam)
	{
		e._camera = deserialize_camera(buffer, offset);
	}
	if (e.has_light)
	{
		e._light = deserialize_light(buffer, offset);
	}

	e.scripts = NULL; // for stb_ds
	e.scripts_len =  deserialize_int(buffer, offset);
	for (int i = 0; i < e.scripts_len; ++i)
	{
		arrput(e.scripts, deserialize_script(buffer, offset));
	}

	e.children = NULL; // for stb_ds
	e.parent =  deserialize_int(buffer, offset);
	e.children_len = deserialize_int(buffer, offset);
	for (int i = 0; i < e.children_len; ++i)
	{
		arrput(e.children, deserialize_int(buffer, offset));
	}
	return e;
}

// ---- components ----

material* deserialize_material(char* buffer, int* offset)
{
	shader s = deserialize_shader(buffer, offset);
	texture dif  = deserialize_texture(buffer, offset);
	texture spec = deserialize_texture(buffer, offset);

	bee_bool is_trans = deserialize_enum(buffer, offset);
	f32 shininess = deserialize_float(buffer, offset);
	vec2 tile; deserialize_vec2(buffer, offset, tile);
	vec3 tint; deserialize_vec3(buffer, offset, tint);

	bee_bool backfaces = deserialize_enum(buffer, offset);
	char* name = deserialize_str(buffer, offset);
	return add_material(s, dif, spec, is_trans, shininess, tile, tint, backfaces, name);
}

mesh* deserialize_mesh(char* buffer, int* offset)
{
	bee_bool visible = deserialize_enum(buffer, offset);
	char* name = deserialize_str(buffer, offset);
	mesh* m = get_mesh(name);
	m->visible = visible;
	return m;
}

camera deserialize_camera(char* buffer, int* offset)
{
	camera c;
	c.perspective = deserialize_float(buffer, offset);
	c.near_plane  = deserialize_float(buffer, offset);
	c.far_plane   = deserialize_float(buffer, offset);

	deserialize_vec3(buffer, offset, c.front);
	deserialize_vec3(buffer, offset, c.up);
	deserialize_vec3(buffer, offset, c.target);
	return c;
}

light deserialize_light(char* buffer, int* offset)
{
	light l;
	l.type = deserialize_enum(buffer, offset);

	deserialize_vec3(buffer, offset, l.ambient);
	deserialize_vec3(buffer, offset, l.diffuse);
	deserialize_vec3(buffer, offset, l.specular);

	deserialize_vec3(buffer, offset, l.direction);

	l.constant  = deserialize_float(buffer, offset);
	l.linear    = deserialize_float(buffer, offset);
	l.quadratic = deserialize_float(buffer, offset);

	l.cut_off		= deserialize_float(buffer, offset);
	l.outer_cut_off = deserialize_float(buffer, offset);

	return l;
}

// ---- internal structs ----

shader deserialize_shader(char* buffer, int* offset)
{
	// shader s;
	// s.handle	= deserialize_u32(buffer, offset);
	char* vert_name = deserialize_str(buffer, offset);
	char* frag_name = deserialize_str(buffer, offset);
	char* name		= deserialize_str(buffer, offset);
	return add_shader(vert_name, frag_name, name); 
}

texture deserialize_texture(char* buffer, int* offset)
{
	// texture t;
	// t.handle = deserialize_u32(buffer, offset);
	// t.size_x = deserialize_int(buffer, offset);
	// t.size_y = deserialize_int(buffer, offset);
	char* name   = deserialize_str(buffer, offset);
	// t.path   = deserialize_str(buffer, offset);
	return get_texture(name);
}

gravity_script* deserialize_script(char* buffer, int* offset)
{
	char* name = deserialize_str(buffer, offset);
	return get_script(name);
}

// ---- base types ----

int deserialize_int(char* buffer, int* offset)
{
	char bytes[sizeof(int)];
	memcpy(bytes, buffer + *offset, sizeof(int));
	int result = strtol(bytes, NULL, 10);
	
	*offset += sizeof(int);
	printf("deserialized int:  %d\n", result);
	return result;
}

f32 deserialize_float(char* buffer, int* offset)
{
	// @TODO: strtof() prob. reads the next floats numbers
	//		  -> limit it to only reading the needed amount of bytes
	char bytes[sizeof(f32)];
	memcpy(bytes, buffer + *offset, sizeof(f32));
	f32 result = strtof(bytes, NULL); // buffer + *offset
	*offset += sizeof(f32);
	printf("deserialized f32:  %f\n", result);
	return result;
}

u32 deserialize_u32(char* buffer, int* offset)
{
	char bytes[sizeof(u32)];
	memcpy(bytes, buffer + *offset, sizeof(u32));
	u32 result = strtoul(bytes, NULL, 10);
	*offset += sizeof(u32);
	printf("deserialized u32:  %d\n", result);
	return result;
}

char deserialize_enum(char* buffer, int* offset)
{
	char result = buffer[*offset];
	*offset += sizeof(char); // gets serializes as a char
	printf("deserialized enum:  %d\n", result);
	return result;
}

char* deserialize_str(char* buffer, int* offset)
{
	int len = deserialize_int(buffer, offset);

	char* result = malloc(len +1 * sizeof(char));
	assert(result != NULL);
	
	for (int i = 0; i < len; ++i)
	{
		result[i] = buffer[*offset];
		*offset += sizeof(char);
	}
	result[len] = '\0';
	// *offset += sizeof(char);
	printf("deserialized str:  %s\n", result);
	return result;
}

void deserialize_vec2(char* buffer, int* offset, vec2 out)
{
	printf("deserialize vec2 ----\n");
	out[0] = deserialize_float(buffer, offset);
	out[1] = deserialize_float(buffer, offset);
	printf("---------------------\n");
}

void deserialize_vec3(char* buffer, int* offset, vec3 out)
{
	printf("deserialize vec3 ----\n");
	out[0] = deserialize_float(buffer, offset);
	out[1] = deserialize_float(buffer, offset);
	out[2] = deserialize_float(buffer, offset);
	printf("---------------------\n");
}