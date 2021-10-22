#pragma once
#ifndef OBJECT_DATA_H
#define OBJECT_DATA_H

#include "CGLM/cglm.h"

#include "global.h"
#include "gravity_script.h"

//
// ---- assets ----
//

// all information needed about a texture
// "icon_handle" is only available when "EDITOR_ACT" is defined and has the same texture loaded in rgb rather than srgb space for ui, etc.
typedef struct texture
{
	u32 handle;
	int size_x;
	int size_y;
	char* name;
	char* path;
#ifdef EDITOR_ACT
	u32 icon_handle;
#endif

}texture;

// all available types of unifoms to be sent to a shader
typedef enum uniform_type { UNIFORM_INT, UNIFORM_F32, UNIFORM_VEC2, UNIFORM_VEC3, UNIFORM_TEX }uniform_type;
// all information needed about a uniform
// doesnt hold the values as this is for the shader to tell the material what uniforms it can give values to
typedef struct uniform_definition
{
	char* name;
	uniform_type type;
}uniform_def;
// holds value for a uniform described by "def"
typedef struct uniform
{
	// char* name;
	// uniform_type type;
	uniform_def* def;
	union
	{
		int int_val;
		f32 f32_val;
		vec2 vec2_val;
		vec3 vec3_val;
		texture tex_val;
	};
}uniform;

typedef struct shader
{
	u32 handle;
	char* vert_name;
	char* frag_name;
	char* name;

	bee_bool use_lighting;
	uniform_def* uniform_defs;
	int uniform_defs_len;

	bee_bool has_error;

}shader;


// ---- components ----

typedef struct material
{
	shader* shader;

	uniform* uniforms;
	int uniforms_len;

	texture dif_tex;
	texture spec_tex;
	texture norm_tex;

	bee_bool is_transparent;

	f32 shininess;
	vec2 tile;
	vec3 tint;

	bee_bool draw_backfaces;
	char* name;
}material;

typedef struct mesh
{
	u32 vertices_len;
	u32 vertices_elems;
	u32 indices_len;
	u32 indices_elems;

	u32 vao, vbo, ebo;

	bee_bool indexed;
	bee_bool visible;

	char* name;
}mesh;

typedef struct camera
{
	f32 perspective;  // 45.0f;
	f32 near_plane;  //  0.1f;
	f32 far_plane;  //   100.0f;
	
	vec3 front;  // = { 0.0f,  -0.5f, -0.85f };
	vec3 up;	 // = { 0.0f,  1.0f,   0.0f };
	vec3 target; // = { 0.0, 0.0, 0.0 };

}camera;

enum light_type { POINT_LIGHT, DIR_LIGHT, SPOT_LIGHT };
typedef enum light_type light_type;
typedef struct light
{
	// ---- general ----
	int id;
	bee_bool enabled;
	bee_bool cast_shadow;
	u32  shadow_map;
	u32  shadow_fbo;
	mat4 light_space;
	int shadow_map_x, shadow_map_y;

	light_type type;

	vec3 ambient;
	vec3 diffuse;
	f32  dif_intensity;
	vec3 specular;

	// -----------------

	vec3 direction; // for dir & spot light

	f32 constant;	// for point & spot light
	f32 linear;		// for point & spot light
	f32 quadratic;	// for point & spot light

	// spot light
	f32 cut_off;
	f32 outer_cut_off;

}light;


// ---- physics ----
typedef struct collision_info
{
	bee_bool collision;
	vec3 direction; // normal * depth of collision
	int entity_id;
	bee_bool trigger;

}collision_info;

typedef struct box_collider
{
	vec3 aabb[2];

}box_collider;
typedef struct sphere_collider
{
	f32 radius;
	// is_trigger, etc.

}sphere_collider;

typedef enum collider_type { SPHERE_COLLIDER, BOX_COLLIDER }collider_type;
typedef struct collider
{
	collider_type type;
	vec3 offset;
	bee_bool is_trigger; // @TODO: implement
	// vec3 scale;
	union
	{
		sphere_collider sphere;
		box_collider	box;
	};

	collision_info* infos;
	int infos_len;

}collider;

typedef struct rigidbody
{
	vec3 velocity;
	vec3 force;
	f32  mass;
}rigidbody;

// ---- entity ----

// struct defing an object or entity
// all meshes that get rendered and also lights cameras and scripts work through these entities
// if "_mesh", "_light", "_camera" are "NULL" there not assigned, this is indicated by the "has_..." variables
// when parent is 9999, or less than 0, the entity has no parent 
typedef struct entity
{
	char* name;
	int   id;
	int   id_idx;

	bee_bool has_trans;
	vec3 pos;
	vec3 rot;
	mat4 rot_mat;
	vec3 scale;

	bee_bool rotate_global;


	// ---- components ----

	bee_bool has_model;
	mesh _mesh;
	material* _material;

	bee_bool has_cam;
	bee_bool has_light;
	union // never going to have a cam with lights and vice versa
	{
		camera _camera;
		light  _light;
	};

	bee_bool  has_collider;
	collider  collider;
	bee_bool  has_rb;
	rigidbody rb;

	// --------------------

	gravity_script** scripts;
	int scripts_len;

	
	int  parent;
	int* children;
	int  children_len;


}entity;

typedef struct scene
{
	f32 exposure;
	bee_bool use_msaa; // multisampled anti-aliasing
	vec3 bg_color;

	// entitities
	entity* entities;
	int entities_len;

}scene;



// creates a material struct
material make_material(shader* s, texture dif_tex, texture spec_tex, texture norm_tex, bee_bool is_transparent, f32 shininess, vec2 tile, vec3 tint, bee_bool draw_backfaces, int uniforms_len, uniform* uniforms, const char* name);
// creates a mesh struct 
// dont do this manually
mesh make_mesh(f32* vertices, int vertices_len, u32* indices, int indices_len, const char* name);
// straightforward mesh of a quad aka. plane
mesh make_plane_mesh();
// straightforward mesh of a cube, you could say the default cube
mesh make_cube_mesh();
// this is unfinished have to finish this
mesh make_grid_mesh(int x_verts, int z_verts);

// create a camera struct
camera make_camera(f32 perspective, f32 near_plane, f32 far_plane);

// create a point light
light make_point_light(vec3 ambient, vec3 diffuse, vec3 specular, f32 constant, f32 linear, f32 quadratic);
// create a directional light
light make_dir_light(vec3 ambient, vec3 diffuse, vec3 specular, vec3 direction);
// create a spot light
light make_spot_light(vec3 ambient, vec3 diffuse, vec3 specular, vec3 direction, f32 constant, f32 linear, f32 quadratic, f32 cut_off, f32 outer_cut_off);

// create a rigidbody
rigidbody make_rigidbody(f32 mass);
// create a sphere collider
collider make_sphere_collider(f32 radius, bee_bool is_trigger);
// create a box collider
collider make_box_collider(vec3 size, bee_bool is_trigger);

// create an entity
entity make_entity(vec3 pos, vec3 rot, vec3 scale, mesh* _mesh, material* mat, camera* cam, light* _light, rigidbody* rb, collider* col, char* _name); //model* _model

// updates all attached components
void update_entity(entity* ent);
// void entity_add_script(entity* ent,const char* path);

// void free_material(material* mat);
// free the buffer objects of the mesh
void free_mesh(mesh* _mesh);
// tell opengl to free the texture
void free_texture(texture* tex);
// tell opengl to free the shader
void free_shader(shader* s);
// free all attached data
void free_entity(entity* ent);



#endif