#pragma once
#ifndef OBJECT_DATA_H
#define OBJECT_DATA_H

#include "CGLM/cglm.h"

#include "global.h"
#include "gravity_script.h"

//
// ---- assets ----
//

typedef struct
{
	u32 handle;
	char* frag_name;
	char* vert_name;
	char* name;

}shader;

typedef struct
{
	u32 handle;
	int size_x;
	int size_y;
	char* path;
	char* name;
}texture;

typedef struct
{
	shader shader;

	texture dif_tex;
	texture spec_tex;

	bee_bool is_transparent;

	f32 shininess;
	vec2 tile;
	vec3 tint;

	bee_bool draw_backfaces;

	char* name;
}material;

typedef struct
{
	u32 vertices_len;
	u32 indices_len;

	u32 vao, vbo, ebo;

	enum bee_bool indexed;
	enum bee_bool visible;

	char* name;
}mesh;

enum light_type { POINT_LIGHT, DIR_LIGHT, SPOT_LIGHT };
typedef enum light_type light_type;
typedef struct
{
	// ---- general ----

	light_type type;

	vec3 ambient;
	vec3 diffuse;
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

// ---- entity ----

typedef struct
{
	char* name;

	bee_bool has_trans;
	vec3 pos;
	vec3 rot;
	vec3 scale;

	bee_bool rotate_global;


	// ---- components ----

	bee_bool has_model;
	mesh _mesh;
	material* _material;

	bee_bool has_light;
	light _light;

	// --------------------

	int scripts_len;
	gravity_script** scripts;


}entity;

// creates a material struct
material make_material(shader s, texture dif_tex, texture spec_tex, bee_bool is_transparent, f32 shininess, vec2 tile, bee_bool draw_backfaces, const char* name);
// creates a material struct, tints the material
material make_material_tint(shader s, texture dif_tex, texture spec_tex, bee_bool is_transparent, f32 shininess, vec2 tile, vec3 tint, bee_bool draw_backfaces, const char* name);

// creates a mesh struct 
// dont do this manually
mesh make_mesh(f32* vertices, int vertices_len, u32* indices, int indices_len, const char* name);
mesh make_plane_mesh();
mesh make_cube_mesh();
mesh make_grid_mesh(int x_verts, int z_verts);

// creates a mesh struct 
// model make_model(mesh* _mesh, material* _material);

// create a point light
light make_point_light(vec3 ambient, vec3 diffuse, vec3 specular, f32 constant, f32 linear, f32 quadratic);
// create a directional light
light make_dir_light(vec3 ambient, vec3 diffuse, vec3 specular, vec3 direction);
// create a spot light
light make_spot_light(vec3 ambient, vec3 diffuse, vec3 specular, vec3 direction, f32 constant, f32 linear, f32 quadratic, f32 cut_off, f32 outer_cut_off);

// create an entity
entity make_entity(vec3 pos, vec3 rot, vec3 scale, mesh* _mesh, material* mat, light* _light, char* _name); //model* _model

// render the mesh
// void draw_mesh(mesh* _mesh, material* _material, vec3 pos, vec3 rot, vec3 scale, enum bee_bool rotate_global);
// render the models meshes
// void draw_model(model* _model, vec3 pos, vec3 rot, vec3 scale, enum bee_bool rotate_global);
// updates all attached components
void update_entity(entity* ent);
// void entity_add_script(entity* ent,const char* path);

void free_material(material* mat);
// free the buffer objects of the mesh
void free_mesh(mesh* _mesh);
// free all attached data
void free_entity(entity* ent);


#endif