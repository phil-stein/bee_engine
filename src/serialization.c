#include "serialization.h"
#include "asset_manager.h"
#include "stb/stb_ds.h"

#include "renderer.h" // tmp
#include "framebuffer.h"
#include "scene_manager.h" // tmp

#define VERSION 1.6f
f32 current_version = VERSION;


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
	// serialization version number
	serialize_float(buffer, offset, VERSION);

	// serialize scene settings
	// @TODO: cube-map
	serialize_float(buffer, offset, s->exposure); // v1.3
	serialize_enum(buffer, offset, s->use_msaa);  // v1.3
	serialize_vec3(buffer, offset, s->bg_color);  // v1.3

	// serializing assets that arent defined in files
	
	int shaders_len = 0;
	shader* shaders = get_all_shaders(&shaders_len);
	// printf("serialized %d shaders\n", shaders_len);

	serialize_int(buffer, offset, shaders_len);

	for (int i = 0; i < shaders_len; ++i)
	{
		serialize_shader(buffer, offset, &shaders[i]);
	}

	int materials_len = 0;
	material* materials = get_all_materials(&materials_len);
	
	serialize_int(buffer, offset, materials_len);
	// printf("serialized %d materials\n", materials_len);

	for (int i = 0; i < materials_len; ++i)
	{
		serialize_material(buffer, offset, &materials[i]);
	}

	// printf("serialized %d entities\n", s->entities_len);
	serialize_int(buffer, offset, s->entities_len);
	for (int i = 0; i < s->entities_len; ++i)
	{
		serialize_entity(buffer, offset, &s->entities[i]);
	}
	
}

void serialize_entity(char* buffer, int* offset, entity* ent)
{
	// id_idx dont need to get serialized
	serialize_str(buffer, offset, ent->name);
	serialize_int(buffer, offset, ent->id);

	serialize_enum(buffer, offset, ent->visible);

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
		serialize_str(buffer, offset, ent->_mesh->name);
		// serialize_mesh(buffer, offset, ent->_mesh);
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

	// v1.4
	serialize_enum(buffer, offset, ent->has_rb);
	if (ent->has_rb)
	{
		serialize_rigidbody(buffer, offset, &ent->rb);
	}
	// v1.4
	serialize_enum(buffer, offset, ent->has_collider);
	if (ent->has_collider)
	{
		serialize_collider(buffer, offset, &ent->collider);
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
	// serialize_shader(buffer, offset, &m->shader);
	serialize_str(buffer, offset, m->name);
	serialize_str(buffer, offset, m->shader->name);
	serialize_texture(buffer, offset, &m->dif_tex);
	serialize_texture(buffer, offset, &m->spec_tex);
	serialize_texture(buffer, offset, &m->norm_tex);

	serialize_enum(buffer, offset, m->is_transparent);
	serialize_float(buffer, offset, m->shininess);
	serialize_vec2(buffer, offset, m->tile);
	serialize_vec3(buffer, offset, m->tint);

	serialize_enum(buffer, offset, m->draw_backfaces);

	serialize_int(buffer, offset, m->uniforms_len);
	for (int i = 0; i < m->uniforms_len; ++i)
	{
		int idx = 0;
		for (int n = 0; n < m->shader->uniform_defs_len; ++n)
		{
			if (m->uniforms[n].def = &m->shader->uniform_defs[n].name)
			{
				idx = n;
				break;
			}
		}
		serialize_uniform(buffer, offset, &m->uniforms[i], idx);
	}
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
	// serialize_enum(buffer, offset, m->visible);
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
	// id doesnt have to be serialized
	serialize_enum(buffer, offset, l->enabled); // v0.5
	serialize_enum(buffer, offset, l->cast_shadow); // v1.0
	serialize_int(buffer, offset, l->shadow_map_x); // v1.2
	serialize_int(buffer, offset, l->shadow_map_y); // v1.2
	serialize_enum(buffer, offset, l->type);

	serialize_vec3(buffer, offset, l->ambient);
	serialize_vec3(buffer, offset, l->diffuse);
	serialize_float(buffer, offset, l->dif_intensity);
	serialize_vec3(buffer, offset, l->specular);

	serialize_vec3(buffer, offset, l->direction);

	serialize_float(buffer, offset, l->constant);
	serialize_float(buffer, offset, l->linear);
	serialize_float(buffer, offset, l->quadratic);

	serialize_float(buffer, offset, l->cut_off);
	serialize_float(buffer, offset, l->outer_cut_off);
}

void serialize_rigidbody(char* buffer, int* offset, rigidbody* rb)
{
	// not serializing velocity as always 0
	serialize_vec3(buffer, offset, rb->force);
	serialize_float(buffer, offset, rb->mass);
}

void serialize_collider(char* buffer, int* offset, collider* c)
{
	serialize_enum(buffer, offset, c->type);
	serialize_vec3(buffer, offset, c->offset);
	if (c->type == SPHERE_COLLIDER)
	{
		serialize_float(buffer, offset, c->sphere.radius);
	}
	else if (c->type == BOX_COLLIDER)
	{
		serialize_vec3(buffer, offset, c->box.aabb[0]);
		serialize_vec3(buffer, offset, c->box.aabb[1]);
	}
	serialize_enum(buffer, offset, c->is_trigger); // v1.5
}

// ---- internal structs ----

void serialize_shader(char* buffer, int* offset, shader* s)
{
	// serialize_u32(buffer, offset, s->handle);
	serialize_str(buffer, offset, s->vert_name);
	serialize_str(buffer, offset, s->frag_name);
	serialize_str(buffer, offset, s->name);

	// v0.6
	serialize_enum(buffer, offset, s->use_lighting);
	serialize_int(buffer, offset, s->uniform_defs_len);
	for (int i = 0; i < s->uniform_defs_len; ++i)
	{
		serialize_uniform_def(buffer, offset, &s->uniform_defs[i]);
	}
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

void serialize_uniform(char* buffer, int* offset, uniform* u, int idx)
{
	serialize_int(buffer, offset, idx);
	switch (u->def->type)
	{
		case UNIFORM_INT:
			serialize_int(buffer, offset, u->int_val);
			break;
		case UNIFORM_F32:
			serialize_float(buffer, offset, u->f32_val);
			break;
		case UNIFORM_VEC2:
			serialize_vec2(buffer, offset, u->vec2_val);
			break;
		case UNIFORM_VEC3:
			serialize_vec3(buffer, offset, u->vec3_val);
			break;
		case UNIFORM_TEX:
			serialize_texture(buffer, offset, &u->tex_val);
			break;
	}

}

void serialize_uniform_def(char* buffer, int* offset, uniform_def* u)
{
	serialize_str(buffer, offset, u->name);
	serialize_enum(buffer, offset, u->type);
}

// ---- base types ----

void serialize_int(char* buffer, int* offset, int val)
{
	// write to buffer
	// printf("serialize int: %d\n", val);
	sprintf(buffer + *offset, "%d", val);

	// move the offset to after the newly written data
	*offset += sizeof(int);
}

void serialize_float(char* buffer, int* offset, f32 val)
{
	// write to buffer
	char buf[20];
	sprintf_s(buf, 20, "%f", val);
	int len = strlen(buf);
	serialize_int(buffer, offset, len);
	sprintf(buffer + *offset, "%s", buf);

	// move the offset to after the newly written data
	*offset += len;
}

void serialize_u32(char* buffer, int* offset, u32 val)
{
	// write to buffer
	// printf("serialize u32: %d\n", val);
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

	// printf("serialize str: ");;
	for (int i = 0; i < len; ++i)
	{
		buffer[*offset] = val[i];
		// printf("%c", buffer[*offset]);
		*offset += sizeof(char);
	}
	// printf("\n");
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

scene deserialize_scene(char* buffer, int* offset, rtn_code* success)
{
	// serialization version number
	current_version = deserialize_float(buffer, offset);
	printf("deserializing scene using version: %f\n", current_version);

	scene s;
	if (current_version >= 1.3f)
	{
		s.exposure = deserialize_float(buffer, offset);
		s.use_msaa = deserialize_enum(buffer, offset);
		vec3 bg;
		deserialize_vec3(buffer, offset, bg);
		glm_vec3_copy(bg, s.bg_color);

	}
	else
	{
		s.exposure = 1;
		s.use_msaa = BEE_TRUE;
		vec3 bg = { 0.1f, 0.1f, 0.1f };
		glm_vec3_copy(bg, s.bg_color);
	}

	// deserialize assets that arent defined in their own files
	int shaders_len = deserialize_int(buffer, offset);
	// printf("derialized %d shaders\n", shaders_len);

	for (int i = 0; i < shaders_len; ++i)
	{
		deserialize_shader(buffer, offset); // automatically adds to the asset-manager
	}

	int materials_len = deserialize_int(buffer, offset);
	// printf("derialized %d materials\n", materials_len);

	for (int i = 0; i < materials_len; ++i)
	{
		deserialize_material(buffer, offset);  // automatically adds to the asset-manager
	}

	// deserialize entities
	s.entities = NULL;
	s.entities_len = deserialize_int(buffer, offset);
	// printf("derialized %d entities\n", s.entities_len);
	for (int i = 0; i < s.entities_len; ++i)
	{
		arrput(s.entities, deserialize_entity(buffer, offset));
		// printf("deserialized entity: %s, id: %d\n", s.entities[i].name, s.entities[i].id);
	}

	*success = BEE_OK;
	return s;
}

entity deserialize_entity(char* buffer, int* offset)
{
	entity e;

	e.name = deserialize_str(buffer, offset);
	printf("deserialized entity.name: %s\n", e.name);

	if (current_version >= 0.2f)
	{
		e.id = deserialize_int(buffer, offset);
	}
	else { e.id = -1; }
	if (current_version >= 1.6f)
	{
		e.visible = deserialize_enum(buffer, offset);
		printf("deserialized entity.visible: %d\n", e.visible);
	}
	else { e.visible = BEE_TRUE; }

	e.has_trans = deserialize_enum(buffer, offset);
	printf("deserialized entity.has_trans: %d\n", e.has_trans);
	if (e.has_trans)
	{
		deserialize_vec3(buffer, offset, e.pos);
		deserialize_vec3(buffer, offset, e.rot);
		deserialize_vec3(buffer, offset, e.scale);
	}
	e.rotate_global = deserialize_enum(buffer, offset);

	e.has_model = deserialize_enum(buffer, offset);
	printf("deserialized entity.has_model: %d\n", e.has_model);
	if (e.has_model)
	{
		if (current_version <= 1.5f)
		{
			e._mesh     = deserialize_mesh(buffer, offset);
		}
		else
		{
			e._mesh = get_mesh(deserialize_str(buffer, offset));
		}
		e._material = get_material(deserialize_str(buffer, offset));
	}

	e.has_cam   = deserialize_enum(buffer, offset);
	e.has_light = deserialize_enum(buffer, offset);
	printf("deserialized entity.has_cam: %d\n", e.has_cam);
	printf("deserialized entity.has_light: %d\n", e.has_light);
	if (e.has_cam)
	{
		e._camera = deserialize_camera(buffer, offset);
	}
	if (e.has_light)
	{
		e._light = deserialize_light(buffer, offset);
	}

	if (current_version >= 1.4f)
	{
		e.has_rb = deserialize_enum(buffer, offset);
		if (e.has_rb)
		{
			e.rb = deserialize_rigidbody(buffer, offset);
		}
		e.has_collider = deserialize_enum(buffer, offset);
		if (e.has_collider)
		{
			e.collider = deserialize_collider(buffer, offset);
		}
	}
	else
	{
		e.has_collider = BEE_FALSE;
		e.has_rb = BEE_FALSE;
	}

	e.scripts = NULL; // for stb_ds
	e.scripts_len =  deserialize_int(buffer, offset);
	for (int i = 0; i < e.scripts_len; ++i)
	{
#ifdef EDITOR_ACT
		gravity_script* script = deserialize_script(buffer, offset);
		script->active = get_gamestate();
		arrput(e.scripts, script);
#else 
		arrput(e.scripts, deserialize_script(buffer, offset));
#endif
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
	char* name = NULL;
	if (current_version >= 0.4f)
	{
		name = deserialize_str(buffer, offset);
		// printf("deserialized material %s\n", name);
	}
	char* shader_name = deserialize_str(buffer, offset);
	shader* s = get_shader(shader_name);
	texture dif  = deserialize_texture(buffer, offset);
	texture spec = deserialize_texture(buffer, offset);
	texture norm = get_texture("blank.png");
	if (current_version >= 0.9f)
	{
		norm = deserialize_texture(buffer, offset);;
	}

	bee_bool is_trans = deserialize_enum(buffer, offset);
	f32 shininess = deserialize_float(buffer, offset);
	vec2 tile; deserialize_vec2(buffer, offset, tile);
	vec3 tint; deserialize_vec3(buffer, offset, tint);

	bee_bool backfaces = deserialize_enum(buffer, offset);
	int uniforms_len = 0;
	uniform* uniforms = NULL;
	if (current_version >= 0.6f)
	{
		uniforms_len = deserialize_int(buffer, offset);
		// if (uniforms_len > 0) { printf(" -> with %d uniforms\n", uniforms_len); }
		for (int i = 0; i < uniforms_len; ++i)
		{
			uniform u = deserialize_uniform(buffer, offset, s);
			arrput(uniforms, u);

		}
	}
	if (current_version <= 0.3f)
	{
		name = deserialize_str(buffer, offset);
	}
	return add_material_specific(s, dif, spec, norm, is_trans, shininess, tile, tint, backfaces, uniforms_len, uniforms, name, BEE_TRUE);
}

mesh* deserialize_mesh(char* buffer, int* offset)
{
	bee_bool visible = deserialize_enum(buffer, offset);
	char* name = deserialize_str(buffer, offset);
	printf("deserialized mesh.name: %s\n", name);
	mesh* m = get_mesh(name);
	// m->visible = visible;
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
	if (current_version >= 0.5f)
	{
		l.enabled = deserialize_enum(buffer, offset);
	}
	else { l.enabled = BEE_TRUE; }
	if (current_version >= 1.0f)
	{
		l.cast_shadow = deserialize_enum(buffer, offset);
	}
	else { l.cast_shadow = BEE_TRUE; }
	if (current_version >= 1.2f)
	{
		l.shadow_map_x = deserialize_int(buffer, offset);
		l.shadow_map_y = deserialize_int(buffer, offset);
	}
	else 
	{
		l.shadow_map_x = 2048;
		l.shadow_map_y = 2048;
	}

	l.type = deserialize_enum(buffer, offset);

	deserialize_vec3(buffer, offset, l.ambient);
	deserialize_vec3(buffer, offset, l.diffuse);
	if (current_version >= 1.1f)
	{
		l.dif_intensity  = deserialize_float(buffer, offset);
	}
	else { l.dif_intensity = 1.0f; }
	deserialize_vec3(buffer, offset, l.specular);

	deserialize_vec3(buffer, offset, l.direction);

	l.constant  = deserialize_float(buffer, offset);
	l.linear    = deserialize_float(buffer, offset);
	l.quadratic = deserialize_float(buffer, offset);

	l.cut_off		= deserialize_float(buffer, offset);
	l.outer_cut_off = deserialize_float(buffer, offset);

	create_framebuffer_shadowmap(&l.shadow_map, &l.shadow_fbo, l.shadow_map_x, l.shadow_map_y);

	return l;
}

rigidbody deserialize_rigidbody(char* buffer, int* offset)
{
	rigidbody rb;
	deserialize_vec3(buffer, offset, rb.force);
	rb.mass = deserialize_float(buffer, offset);
	glm_vec3_copy(GLM_VEC3_ZERO, rb.velocity);
	return rb;
}

collider deserialize_collider(char* buffer, int* offset)
{
	collider c;
	c.type = deserialize_enum(buffer, offset);
	deserialize_vec3(buffer, offset, c.offset);
	if (c.type == SPHERE_COLLIDER)
	{
		sphere_collider s;
		s.radius = deserialize_float(buffer, offset);
		c.sphere = s;
	}
	else if (c.type == BOX_COLLIDER)
	{
		box_collider b;
		deserialize_vec3(buffer, offset, b.aabb[0]);
		deserialize_vec3(buffer, offset, b.aabb[1]);
		c.box = b;
	}
	if (current_version >= 1.5f)
	{
		c.is_trigger = deserialize_enum(buffer, offset);
	}
	else { c.is_trigger = BEE_FALSE; }

	c.infos = NULL;
	c.infos_len = 0;

	return c;
}

// ---- internal structs ----

shader* deserialize_shader(char* buffer, int* offset)
{
	// shader s;
	// s.handle	= deserialize_u32(buffer, offset);
	char* vert_name = deserialize_str(buffer, offset);
	char* frag_name = deserialize_str(buffer, offset);
	char* name		= deserialize_str(buffer, offset);
	// printf("deserialized shader %s\n", name);
	if (current_version >= 0.7f)
	{
		bee_bool use_lighting = deserialize_enum(buffer, offset);
		int uniform_defs_len = deserialize_int(buffer, offset);
		// if (uniform_defs_len > 0) { printf(" -> with %d uniform types\n", uniform_defs_len); }
		uniform_def* uniform_defs = NULL;
		for (int i = 0; i < uniform_defs_len; ++i)
		{
			uniform_def def = deserialize_uniform_def(buffer, offset);
			arrput(uniform_defs, def);
		}
		return add_shader_specific(vert_name, frag_name, name, use_lighting, uniform_defs_len, uniform_defs, BEE_FALSE); // BEE_TRUE
	}
	else if (current_version >= 0.6f)
	{
		deserialize_enum(buffer, offset);
		int uniform_types_len = deserialize_int(buffer, offset);
		for (int i = 0; i < uniform_types_len; ++i)
		{
			deserialize_enum(buffer, offset);
		}
	}
	else if (current_version >= 0.3f)
	{
		deserialize_enum(buffer, offset);
		int uniforms_len = deserialize_int(buffer, offset);
		for (int i = 0; i < uniforms_len; ++i)
		{
			deserialize_uniform(buffer, offset, get_shader(name));
		}
	}
	return add_shader(vert_name, frag_name, name, BEE_FALSE); // BEE_TRUE
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
	gravity_script* s = get_script(name);
	// s->init_closure_assigned = BEE_FALSE;
	// s->closure = NULL;
	// s->vm = NULL;
	return s;
}

uniform deserialize_uniform(char* buffer, int* offset, shader* s)
{
	uniform u;
	if (current_version >= 0.8f)
	{
		int idx = deserialize_int(buffer, offset);
		u.def = &s->uniform_defs[idx];
		printf("deserialized uniform_def idx: %d\n", idx);
	}
	else if (current_version >= 0.7f || 1) // tmp
	{
		u.def = NULL;
		deserialize_int(buffer, offset);
	}
	else
	{
		u.def = NULL;
		char* n = deserialize_str(buffer, offset);
		char  t = deserialize_enum(buffer, offset);
		return u;
	}
	switch (u.def->type)
	{
		case UNIFORM_INT:
			u.int_val = deserialize_int(buffer, offset);
			break;
		case UNIFORM_F32:
			u.f32_val = deserialize_float(buffer, offset);
			break;
		case UNIFORM_VEC2:
			deserialize_vec2(buffer, offset, u.vec2_val);
			break;
		case UNIFORM_VEC3:
			deserialize_vec3(buffer, offset, u.vec3_val);
			break;
		case UNIFORM_TEX:
			u.tex_val = deserialize_texture(buffer, offset);
			break;
	}
	printf("deserialized uniform: def[\"%s\", %d]\n", u.def->name, u.def->type);
	return u;
}

uniform_def deserialize_uniform_def(char* buffer, int* offset)
{
	uniform_def u;
	u.name = deserialize_str(buffer, offset);
	u.type = deserialize_enum(buffer, offset);
	printf("deserialized uniform_def: \"%s\", type: %d\n", u.name, u.type);
	return u;
}

// ---- base types ----

int deserialize_int(char* buffer, int* offset)
{
	char bytes[sizeof(int)];
	memcpy(bytes, buffer + *offset, sizeof(int));
	int result = strtol(bytes, NULL, 10);
	
	*offset += sizeof(int);
	// printf("deserialized int:  %d\n", result);
	return result;
}

f32 deserialize_float(char* buffer, int* offset)
{
	f32 result;
	if (current_version >= 0.8f)
	{
		int len = deserialize_int(buffer, offset);
		char* bytes = malloc(len * sizeof(char));
		memcpy(bytes, buffer + *offset, len);
		result = strtof(bytes, NULL); // buffer + *offset
		free(bytes);
		*offset += len;
	}
	else
	{
		char bytes[sizeof(f32)];
		memcpy(bytes, buffer + *offset, sizeof(f32));
		result = strtof(bytes, NULL); // buffer + *offset
		*offset += sizeof(f32);
	}
	// printf("deserialized f32:  %f\n", result);
	return result;
}

u32 deserialize_u32(char* buffer, int* offset)
{
	char bytes[sizeof(u32)];
	memcpy(bytes, buffer + *offset, sizeof(u32));
	u32 result = strtoul(bytes, NULL, 10);
	*offset += sizeof(u32);
	// printf("deserialized u32:  %d\n", result);
	return result;
}

char deserialize_enum(char* buffer, int* offset)
{
	char result = buffer[*offset];
	*offset += sizeof(char); // gets serializes as a char
	// printf("deserialized enum:  %d\n", result);
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
	// printf("deserialized str:  %s\n", result);
	return result;
}

void deserialize_vec2(char* buffer, int* offset, vec2 out)
{
	// printf("deserialize vec2 ----\n");
	out[0] = deserialize_float(buffer, offset);
	out[1] = deserialize_float(buffer, offset);
	// printf("---------------------\n");
}

void deserialize_vec3(char* buffer, int* offset, vec3 out)
{
	// printf("deserialize vec3 ----\n");
	out[0] = deserialize_float(buffer, offset);
	out[1] = deserialize_float(buffer, offset);
	out[2] = deserialize_float(buffer, offset);
	// printf("---------------------\n");
}