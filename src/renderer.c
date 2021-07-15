#include "renderer.h"

#define STB_DS_IMPLEMENTATION
#include "stb/stb_ds.h"
#include "stb/stb_image.h"
#include "GLAD/glad.h"

#include "shader.h"
#include "window.h"
#include "camera.h"
#include "framebuffer.h"
#include "file_handler.h"
#include "asset_manager.h"


#define BLEND_SORT_DEPTH 3


// ---- vars ----

// camera
f32 perspective = 45.0f;
f32 near_plane  = 0.1f;
f32 far_plane   = 100.0f;

// entities
entity* entities;
int entities_len = 0;
// size_t entities_size = 0;
// bee_bool entity_arr_init = BEE_FALSE;

int* transparent_ents     = NULL;
int  transparent_ents_len = 0;

// lights
int* dir_lights		 = NULL;
int dir_lights_len   = 0;
int* point_lights	 = NULL;
int point_lights_len = 0;
int* spot_lights	 = NULL;
int spot_lights_len  = 0;

// different modes
bee_bool wireframe_mode_enabled = BEE_FALSE;
bee_bool normal_mode_enabled	 = BEE_FALSE;
bee_bool uv_mode_enabled		 = BEE_FALSE;
vec3 wireframe_color = { 0.0f, 0.0f, 0.0f };

shader modes_shader;

// framebuffer
mesh m;
shader screen_shader;
u32 tex_col_buffer;
u32 quad_vao, quad_vbo;

// skybox
u32 cube_map;
shader skybox_shader;
u32 skybox_vao, skybox_vbo;
bee_bool draw_skybox = BEE_TRUE;


void renderer_init()
{
	// framebuffer ----------------------------------------------------------------------------------
	// m = make_plane_mesh();
	// create_shader_from_file used before
	screen_shader = add_shader("screen.vert", // C:\\Workspace\\C\\BeeEngine\\assets\\shaders\\screen.vert"
							   "screen.frag", "SHADER_framebuffer"); // C:\\Workspace\\C\\BeeEngine\\assets\\shaders\\screen.frag"

	create_framebuffer(&tex_col_buffer);
	set_framebuffer_to_update(&tex_col_buffer); // updates framebuffer on window resize

	f32 quad_verts[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	// positions   // texCoords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
	};
	// screen quad VAO
	glGenVertexArrays(1, &quad_vao);
	glGenBuffers(1, &quad_vbo);
	glBindVertexArray(quad_vao);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(f32), &quad_verts, GL_STATIC_DRAW); // quad_verts is 24 long
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(f32), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(f32), (void*)(2 * sizeof(f32)));
	// ----------------------------------------------------------------------------------------------

	// cube map -------------------------------------------------------------------------------------

	// create_shader_from_file used before
	skybox_shader = add_shader("cube_map.vert", // "C:\\Workspace\\C\\BeeEngine\\assets\\shaders\\cube_map.vert"
							   "cube_map.frag", "SHADER_skybox"); // "C:\\Workspace\\C\\BeeEngine\\assets\\shaders\\cube_map.frag"

	cube_map = load_cubemap();

	float skybox_verts[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	//  cube vao
	glGenVertexArrays(1, &skybox_vao);
	glGenBuffers(1, &skybox_vbo);
	glBindVertexArray(skybox_vao);
	glBindBuffer(GL_ARRAY_BUFFER, skybox_vbo);
	glBufferData(GL_ARRAY_BUFFER, 108 * sizeof(f32), &skybox_verts, GL_STATIC_DRAW); // skybox_verts is 108 long
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void*)0);
	// ----------------------------------------------------------------------------------------------

#ifdef EDITOR_ACT
	// create_shader_from_file used before
	modes_shader = add_shader("basic.vert", //"C:\\Workspace\\C\\BeeEngine\\assets\\shaders\\basic.vert"
							  "modes.frag", "SHADER_modes"); // "C:\\Workspace\\C\\BeeEngine\\assets\\shaders\\modes.frag"
#endif
#ifndef EDITOR_ACT
	// set opengl state, only once in build mode as we dont use nuklear
	glEnable(GL_DEPTH_TEST); // enable the z-buffer
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND); // enable blending of transparent texture
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //set blending function: 1 - source_alpha, e.g. 0.6(60%) transparency -> 1 - 0.6 = 0.4(40%)
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
#endif

	// set background-color
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

void renderer_update()
{
	// first pass
	bind_framebuffer();
	// glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear bg
	
#ifdef EDITOR_ACT
	// need to restore the opengl state after calling nuklear
	glEnable(GL_DEPTH_TEST); // enable the z-buffer
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND); // enable blending of transparent texture
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //set blending function: 1 - source_alpha, e.g. 0.6(60%) transparency -> 1 - 0.6 = 0.4(40%)
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
#endif

#ifdef EDITOR_ACT
	if (wireframe_mode_enabled == BEE_FALSE)
	{
		// draw in solid-mode
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else
	{
		// draw in wireframe-mode
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	}
#endif

	// draw opaque objects
	for (int i = 0; i < entities_len; ++i)
	{
		// skip transparent objects
		bee_bool is_trans = BEE_FALSE;
		for (int n = 0; n < transparent_ents_len; ++n)
		{
			if (i == transparent_ents[n]) { is_trans = BEE_TRUE; break; }
		}
		if (is_trans) { continue; }

		if (entities[i].has_model)
		{
			draw_mesh(&entities[i]._mesh, entities[i]._material, entities[i].pos, entities[i].rot, entities[i].scale, entities[i].rotate_global);
		}
	}

	// sort transparent / translucent objects
	vec3 cam_pos; get_camera_pos(&cam_pos);
	for(int rpt = 0; rpt < BLEND_SORT_DEPTH; ++rpt)
	{
		// sort by distance
		for (int i = 0; (i + 1) < transparent_ents_len; ++i)
		{
			// calc dist for each transparent obj
			f32 dist01 = glm_vec3_distance(cam_pos, entities[transparent_ents[i]].pos);
			f32 dist02 = glm_vec3_distance(cam_pos, entities[transparent_ents[i + 1]].pos);
			if (dist02 > dist01)
			{
				// switch places
				int index = transparent_ents[i + 1];
				arrins(transparent_ents, i, index);
				// remove old instance 
				arrdel(transparent_ents, i + 2);
			}
		}
	}

	// skybox -----------------------------------------------------------------
#ifdef EDITOR_ACT
	// draw in solid-mode for skybox
	if (wireframe_mode_enabled == BEE_TRUE)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
#endif
	if (draw_skybox == BEE_TRUE)
	{
		// draw skybox as last
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		//shader_use(skybox_shader);
		shader_use(skybox_shader);

		mat4 view;
		get_camera_view_mat(&view[0]);
		mat3 conv;
		glm_mat4_pick3(view, conv);	// copy to mat3 to remove the position part
		glm_mat4_zero(view);		// reset view
		glm_mat4_ins3(conv, view);	// conv back to mat4


		mat4 proj;
		f32 deg_pers = perspective; glm_make_rad(&deg_pers);
		int w, h; get_window_size(&w, &h);
		glm_perspective(deg_pers, ((float)w / (float)h), near_plane, far_plane, proj);

		shader_set_mat4(skybox_shader, "view", &view[0]);
		shader_set_mat4(skybox_shader, "proj", &proj[0]);

		// skybox cube
		glBindVertexArray(skybox_vao);
		glActiveTexture(GL_TEXTURE0);
		shader_set_int(skybox_shader, "cube_map", 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cube_map);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default
	}
	// ------------------------------------------------------------------------

	// draw transparent objects -----------------------------------------------
#ifdef EDITOR_ACT
	// change back after skybox
	if (wireframe_mode_enabled == BEE_TRUE)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
#endif
	// doing this after the skyboy so the skybox can be seen through the transparent objects
	for (int n = 0; n < transparent_ents_len; ++n)
	{
		int i = transparent_ents[n];
		if (entities[i].has_model)
		{
			draw_mesh(&entities[i]._mesh, entities[i]._material, entities[i].pos, entities[i].rot, entities[i].scale, entities[i].rotate_global);
		}
	}
	// ------------------------------------------------------------------------

	// framebuffer ------------------------------------------------------------
#ifdef EDITOR_ACT
	// draw in solid-mode for fbo
	if (wireframe_mode_enabled == BEE_TRUE)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
#endif

	// second pass
	unbind_framebuffer();
	// glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(screen_shader.handle);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_col_buffer);
	shader_set_int(screen_shader, "tex", 0);
	glBindVertexArray(quad_vao);
	glDisable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	// ------------------------------------------------------------------------

	// entities ---------------------------------------------------------------

	for (int i = 0; i < entities_len; ++i)
	{
		update_entity(&entities[i]);
	}

	// ------------------------------------------------------------------------

#ifdef EDITOR_ACT
	// do this so the nuklear gui is always drawn in solid-mode
	// enable solid-mode in case wireframe-mode is on
	if (wireframe_mode_enabled == BEE_TRUE)
	{ glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }
#endif
}

void draw_mesh(mesh* _mesh, material* mat, vec3 pos, vec3 rot, vec3 scale, enum bee_bool rotate_global)
{
	if (_mesh->visible == BEE_FALSE)
	{
		return;
	}
	if (mat->draw_backfaces == BEE_TRUE)
	{
		glDisable(GL_CULL_FACE);
	}

	mat4 model = GLM_MAT4_IDENTITY_INIT;
	f32 x = rot[0];  glm_make_rad(&x);
	f32 y = rot[1];  glm_make_rad(&y);
	f32 z = rot[2];  glm_make_rad(&z);

	if (rotate_global == BEE_TRUE)
	{
		const vec3 x_axis = { 1.0f, 0.0f, 0.0f };
		const vec3 y_axis = { 0.0f, 1.0f, 0.0f };
		const vec3 z_axis = { 0.0f, 0.0f, 1.0f };
		glm_rotate_at(model, pos, x, x_axis);
		glm_rotate_at(model, pos, y, y_axis);
		glm_rotate_at(model, pos, z, z_axis);
	}
	else
	{
		glm_rotate_x(model, x, model);
		glm_rotate_y(model, y, model);
		glm_rotate_z(model, z, model);
	}

	glm_translate(model, pos);

	glm_scale(model, scale);

	mat4 view;
	get_camera_view_mat(&view[0]);

	mat4 proj;
	f32 deg_pers = perspective; glm_make_rad(&deg_pers);
	int w, h; get_window_size(&w, &h);
	glm_perspective(deg_pers, ((float)w / (float)h), near_plane, far_plane, proj);

#ifdef EDITOR_ACT
	if (wireframe_mode_enabled == BEE_TRUE || normal_mode_enabled == BEE_TRUE || uv_mode_enabled == BEE_TRUE) 
	{
		// act the shader
		shader_use(modes_shader);

		// set shader matrices ------------------------------
		shader_set_mat4(modes_shader, "model", &model[0]);
		shader_set_mat4(modes_shader, "view", &view[0]);
		shader_set_mat4(modes_shader, "proj", &proj[0]);

		vec3 cam_pos; get_camera_pos(cam_pos);
		shader_set_vec3(modes_shader, "viewPos", cam_pos);


		if (wireframe_mode_enabled == BEE_TRUE)
		{
			shader_set_int(modes_shader, "mode", 0);
			shader_set_vec3(modes_shader, "wiref_col", wireframe_color);
		}
		else if (normal_mode_enabled == BEE_TRUE)
		{
			shader_set_int(modes_shader, "mode", 1);
		}
		else if (uv_mode_enabled == BEE_TRUE)
		{
			shader_set_int(modes_shader, "mode", 2);

		}
	}
	else 
	{
#endif
		shader_use(mat->shader);

		// set shader matrices ------------------------------
		shader_set_mat4(mat->shader, "model", &model[0]);
		shader_set_mat4(mat->shader, "view", &view[0]);
		shader_set_mat4(mat->shader, "proj", &proj[0]);

		vec3 cam_pos; get_camera_pos(cam_pos);
		shader_set_vec3(mat->shader, "viewPos", cam_pos);

		// set shader material ------------------------------
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mat->dif_tex.handle);
		shader_set_int(mat->shader, "material.diffuse", 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mat->spec_tex.handle);
		shader_set_int(mat->shader, "material.specular", 1);

		shader_set_float(mat->shader, "material.shininess", mat->shininess);
		shader_set_vec2(mat->shader, "material.tile", mat->tile);
	
		shader_set_vec3(mat->shader, "material.tint", mat->tint);

		// set shader light ---------------------------------
		char buffer[28]; // pointLights[i].quadratic is the longest str at 24
		shader_set_int(mat->shader, "Num_DirLights", dir_lights_len);
		for (int i = 0; i < dir_lights_len; ++i)
		{
			sprintf(buffer, "dirLights[%d].direction", i);
			shader_set_vec3(mat->shader, buffer, entities[dir_lights[i]]._light.direction);

			sprintf(buffer, "dirLights[%d].ambient", i);
			shader_set_vec3(mat->shader, buffer,  entities[dir_lights[i]]._light.ambient);
			sprintf(buffer, "dirLights[%d].diffuse", i);
			shader_set_vec3(mat->shader, buffer,  entities[dir_lights[i]]._light.diffuse);
			sprintf(buffer, "dirLights[%d].specular", i);
			shader_set_vec3(mat->shader, buffer, entities[dir_lights[i]]._light.specular);
		}
		shader_set_int(mat->shader, "Num_PointLights", point_lights_len);
		for (int i = 0; i < point_lights_len; ++i)
		{
			sprintf(buffer, "pointLights[%d].position", i);
			shader_set_vec3(mat->shader, buffer,	  entities[point_lights[i]].pos);

			sprintf(buffer, "pointLights[%d].ambient", i);
			shader_set_vec3(mat->shader, buffer,    entities[point_lights[i]]._light.ambient);
			sprintf(buffer, "pointLights[%d].diffuse", i);
			shader_set_vec3(mat->shader, buffer,    entities[point_lights[i]]._light.diffuse);
			sprintf(buffer, "pointLights[%d].specular", i);
			shader_set_vec3(mat->shader, buffer,   entities[point_lights[i]]._light.specular);
			sprintf(buffer, "pointLights[%d].constant", i);
			shader_set_float(mat->shader, buffer,  entities[point_lights[i]]._light.constant);
			sprintf(buffer, "pointLights[%d].linear", i);
			shader_set_float(mat->shader, buffer,    entities[point_lights[i]]._light.linear);
			sprintf(buffer, "pointLights[%d].quadratic", i);
			shader_set_float(mat->shader, buffer, entities[point_lights[i]]._light.quadratic);
		}
		shader_set_int(mat->shader, "Num_SpotLights", spot_lights_len);
		// printf("amount of spotlights: %d\n", spot_lights_len);
		for (int i = 0; i < spot_lights_len; ++i)
		{
			sprintf(buffer, "spotLights[%d].position", i);
			shader_set_vec3(mat->shader, buffer, entities[spot_lights[i]].pos);

			sprintf(buffer, "spotLights[%d].direction", i);
			shader_set_vec3(mat->shader, buffer, entities[spot_lights[i]]._light.direction);

			sprintf(buffer, "spotLights[%d].ambient", i);
			shader_set_vec3(mat->shader, buffer, entities[spot_lights[i]]._light.ambient);
			sprintf(buffer, "spotLights[%d].diffuse", i);
			shader_set_vec3(mat->shader, buffer, entities[spot_lights[i]]._light.diffuse);
			sprintf(buffer, "spotLights[%d].specular", i);
			shader_set_vec3(mat->shader, buffer, entities[spot_lights[i]]._light.specular);
			sprintf(buffer, "spotLights[%d].constant", i);
			shader_set_float(mat->shader, buffer, entities[spot_lights[i]]._light.constant);
			sprintf(buffer, "spotLights[%d].linear", i);
			shader_set_float(mat->shader, buffer, entities[spot_lights[i]]._light.linear);
			sprintf(buffer, "spotLights[%d].quadratic", i);
			shader_set_float(mat->shader, buffer, entities[spot_lights[i]]._light.quadratic);
			sprintf(buffer, "spotLights[%d].cutOff", i);
			shader_set_float(mat->shader, buffer, entities[spot_lights[i]]._light.cut_off);
			sprintf(buffer, "spotLights[%d].outerCutOff", i);
			shader_set_float(mat->shader, buffer, entities[spot_lights[i]]._light.outer_cut_off);
		}
#ifdef EDITOR_ACT
	}
#endif

	// shader_use(mat->shader);
	glBindVertexArray(_mesh->vao); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
	if (_mesh->indexed == BEE_TRUE)
	{
		glDrawElements(GL_TRIANGLES, (_mesh->indices_len), GL_UNSIGNED_INT, 0);
	}
	else
	{
		glDrawArrays(GL_TRIANGLES, 0, (_mesh->vertices_len / 8)); // each vertices consist of 8 floats
	}
	if (mat->draw_backfaces == BEE_TRUE)
	{
		glEnable(GL_CULL_FACE);
	}
}


void renderer_cleanup()
{
	for (int i = 0; i < entities_len; ++i)
	{
		free_entity(&entities[i]);
	}

	arrfree(entities);
	arrfree(dir_lights);
	arrfree(point_lights);
	arrfree(spot_lights);
	arrfree(transparent_ents);
}

// add an entity
int add_entity(vec3 pos, vec3 rot, vec3 scale, mesh* _mesh, material* _material, light* _light, char* name)
{
	entities_len++;
	arrput(entities, make_entity(pos, rot, scale, _mesh, _material, _light, name));

	if (_material != NULL && _material->is_transparent)
	{
		transparent_ents_len++;
		arrput(transparent_ents, entities_len -1);
	}

	if (_light != NULL)
	{
		switch (_light->type)
		{
			case DIR_LIGHT:
				arrput(dir_lights, entities_len -1);
				dir_lights_len++;
			case POINT_LIGHT:
				arrput(point_lights, entities_len -1);
				point_lights_len++;
			case SPOT_LIGHT:
				arrput(spot_lights, entities_len -1);
				spot_lights_len++;
		}
	}

	return entities_len -1;
}
void add_entity_cube()
{
	mesh m = make_cube_mesh(); // @TODO: make this check with the assetmanager if a cube mesh already exists
	vec3 pos = { 0.0f, 0.0f, 0.0f };
	vec3 rot = { 0.0f, 0.0f, 0.0f };
	vec3 scale = { 1.0f, 1.0f, 1.0f };
	add_entity(pos, rot, scale, &m, &entities[0]._material, NULL, "cube");
}

// doesnt work yet
void entity_switch_light_type(int entity_id, light_type new_type)
{
	// remove from old type categorisation
	switch (entities[entity_id]._light.type)
	{
		case DIR_LIGHT:
			arrdel(dir_lights, entity_id);
			dir_lights_len--;
		case POINT_LIGHT:
			arrdel(point_lights, entity_id);
			point_lights_len--;
		case SPOT_LIGHT:
			arrdel(spot_lights, entity_id);
			spot_lights_len--;
	}

	// add to new type categorisation
	switch (new_type)
	{
		case DIR_LIGHT:
			arrput(dir_lights, entities_len - 1);
			dir_lights_len++;
		case POINT_LIGHT:
			arrput(point_lights, entities_len - 1);
			point_lights_len++;
		case SPOT_LIGHT:
			arrput(spot_lights, entities_len - 1);
			spot_lights_len++;
	}

	entities[entity_id]._light.type = new_type;
}

void entity_add_script(int entity_index, const char* name)
{
	// gravity_script script = make_script(path);
	gravity_script* script = get_script(name);
#ifdef EDITOR_ACT
	script->active = BEE_FALSE;
#endif
	arrput(entities[entity_index].scripts, script);
	entities[entity_index].scripts_len++;

}
void entity_remove_script(int entity_index, int script_index)
{
	free_script(entities[entity_index].scripts[script_index]);
	arrdel(entities[entity_index].scripts, script_index);
	entities[entity_index].scripts_len--;
}
void set_all_scripts(bee_bool act)
{
	for (int i = 0; i < entities_len; ++i)
	{
		for (int n = 0; n < entities[i].scripts_len; ++n)
		{
			entities[i].scripts[n]->active = act;
		}
	}
}


// doesnt work yet
void entity_remove(int entity_idx)
{
	if (entities[entity_idx].has_light)
	{
		switch (entities[entity_idx]._light.type)
		{
			case DIR_LIGHT:
				for (int i = 0; i < dir_lights_len; ++i)
				{
					if (dir_lights[i] == entity_idx)
					{
						arrdel(dir_lights, i);
						dir_lights_len--;
						break;
					}
				}
			case POINT_LIGHT:
				for (int i = 0; i < point_lights_len; ++i)
				{
					if (point_lights[i] == entity_idx)
					{
						arrdel(point_lights, i);
						point_lights_len--;
						break;
					}
				}
			case SPOT_LIGHT:
				for (int i = 0; i < spot_lights_len; ++i)
				{
					if (spot_lights[i] == entity_idx)
					{
						arrdel(spot_lights, i);
						spot_lights_len--;
						break;
					}
				}
		}
	}


	// entity* ptr = &entities[entity_idx];
	arrdel(entities, entity_idx);
	entities_len--;
	// free_entity(ptr);
}

void get_entity_len(int* _entities_len)
{
	*_entities_len = entities_len;
}
entity* get_entites()
{
	return entities;
}
entity* get_entity(int i)
{
	return &entities[i];
}
int get_entity_id_by_name(char* name)
{
	for (int i = 0; i < entities_len; ++i)
	{
		if (strcmp(entities[i].name, name) == 0)
		{
			return i;
		}
	}
	assert(0 == 1);
	return 9999;
}
entity* get_entity_ptr(int idx)
{
	return &entities[idx];
}
// entity_properties was the old way to pass data to the front-end
/*
entity_properties get_entity_properties(int index)
{
	entity_properties prop;

	prop.ent_name		= entities[index].name;
	prop.rotate_global	= &entities[index].rotate_global;
	prop.has_model		= &entities[index].has_model;
	prop.has_light		= &entities[index].has_light;
	prop.has_trans		= &entities[index].has_trans;

	prop.pos_x = &entities[index].pos[0];
	prop.pos_y = &entities[index].pos[1];
	prop.pos_z = &entities[index].pos[2];

	prop.rot_x = &entities[index].rot[0];
	prop.rot_y = &entities[index].rot[1];
	prop.rot_z = &entities[index].rot[2];

	prop.scale_x = &entities[index].scale[0];
	prop.scale_y = &entities[index].scale[1];
	prop.scale_z = &entities[index].scale[2];

	if (entities[index].has_model == BEE_TRUE)
	{
		// mesh
		prop.mesh		  = &entities[index]._mesh;
		prop.mesh_name	  =  entities[index]._mesh.name;
		prop.verts_len    = &entities[index]._mesh.vertices_len;
		prop.indices_len  = &entities[index]._mesh.indices_len;
		prop.mesh_indexed = &entities[index]._mesh.indexed;
		prop.mesh_visible = &entities[index]._mesh.visible;

		// material
		prop.mat			 = &entities[index]._material;
		prop.material_name	 =  entities[index]._material.name;
		prop.shininess		 = &entities[index]._material.shininess;
		prop.tile_x			 = &entities[index]._material.tile[0];
		prop.tile_y			 = &entities[index]._material.tile[1];
		prop.tint_r			 = &entities[index]._material.tint[0];
		prop.tint_g			 = &entities[index]._material.tint[1];
		prop.tint_b			 = &entities[index]._material.tint[2];
		prop.dif_tex_name	 =  entities[index]._material.dif_tex.name;
		prop.spec_tex_name	 =  entities[index]._material.spec_tex.name;
		prop.is_transparent  = &entities[index]._material.is_transparent;
		prop.dif_tex_handle  = &entities[index]._material.dif_tex.handle;
		prop.spec_tex_handle = &entities[index]._material.spec_tex.handle;

	}
	if (entities[index].has_light == BEE_TRUE)
	{
		prop._light_type = &entities[index]._light.type;

		prop.ambient_r = &entities[index]._light.ambient[0];
		prop.ambient_g = &entities[index]._light.ambient[1];
		prop.ambient_b = &entities[index]._light.ambient[2];
		
		prop.diffuse_r = &entities[index]._light.diffuse[0];
		prop.diffuse_g = &entities[index]._light.diffuse[1];
		prop.diffuse_b = &entities[index]._light.diffuse[2];
		
		prop.specular_r = &entities[index]._light.specular[0];
		prop.specular_g = &entities[index]._light.specular[1];
		prop.specular_b = &entities[index]._light.specular[2];
		
		if (entities[index]._light.type == DIR_LIGHT || entities[index]._light.type == SPOT_LIGHT)
		{
			prop.direction_x = &entities[index]._light.direction[0];
			prop.direction_y = &entities[index]._light.direction[1];
			prop.direction_z = &entities[index]._light.direction[2];
		}
		if (entities[index]._light.type == POINT_LIGHT || entities[index]._light.type == SPOT_LIGHT)
		{
			prop.constant	= &entities[index]._light.constant;
			prop.linear		= &entities[index]._light.linear;
			prop.quadratic	= &entities[index]._light.quadratic;
		}
		if (entities[index]._light.type == SPOT_LIGHT)
		{
			prop.cut_off		= &entities[index]._light.cut_off;
			prop.outer_cut_off	= &entities[index]._light.outer_cut_off;
		}
	}


	prop.scripts_len = &entities[index].scripts_len;
	prop.scripts     =  entities[index].scripts;

	return prop;
}
*/

renderer_properties get_renderer_properties()
{
	renderer_properties prop;
	prop.perspective = &perspective;
	prop.near_plane  = &near_plane;
	prop.far_plane   = &far_plane;

	prop.wireframe_col_r = &wireframe_color[0];
	prop.wireframe_col_g = &wireframe_color[1];
	prop.wireframe_col_b = &wireframe_color[2];

	return prop;
}

void renderer_enable_wireframe_mode(bee_bool act)
{
	if (act == BEE_SWITCH)
	{
		wireframe_mode_enabled = !wireframe_mode_enabled;
	}
	else
	{
		wireframe_mode_enabled = act;
	}

	printf("> switching to %s\n", wireframe_mode_enabled == 0 ? "solid-mode" : "wireframe-mode");
}
void renderer_enable_normal_mode(bee_bool act)
{
	if (act == BEE_SWITCH)
	{
		normal_mode_enabled = !normal_mode_enabled;
	}
	else
	{
		normal_mode_enabled = act;
	}

	printf("> switching to %s\n", normal_mode_enabled == 0 ? "default" : "normal-mode");
}
void renderer_enable_uv_mode(bee_bool act)
{
	if (act == BEE_SWITCH)
	{
		uv_mode_enabled = !uv_mode_enabled;
	}
	else
	{
		uv_mode_enabled = act;
	}

	printf("> switching to %s\n", uv_mode_enabled == 0 ? "default" : "uv-mode");
}
void renderer_set_skybox_active(bee_bool act)
{
	if (act == BEE_SWITCH)
	{
		draw_skybox = !draw_skybox;
	}
	else
	{
		draw_skybox = act;
	}

	printf("> switching to %s\n", draw_skybox == 0 ? "drawing skybox" : "not drawing skybox");
}