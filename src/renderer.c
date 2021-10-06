#include "renderer.h"

#define STB_DS_IMPLEMENTATION
#include "stb/stb_ds.h"
#include "stb/stb_image.h"
#include "GLAD/glad.h"

#include "input.h"
#include "shader.h"
#include "window.h"
#include "camera.h"
#include "framebuffer.h"
#include "file_handler.h"
#include "asset_manager.h"
#include "scene_manager.h"
#include "gravity_interface.h"


#define BLEND_SORT_DEPTH 3


// ---- vars ----

#ifdef EDITOR_ACT
bee_bool gamestate = BEE_FALSE; // start of in editor mode
bee_bool hide_gizmos = BEE_TRUE;
#else 
bee_bool gamestate = BEE_TRUE; // start of in game mode
bee_bool hide_gizmos = BEE_TRUE;
#endif
// camera
f32 perspective = 45.0f;
f32 near_plane  = 0.1f;
f32 far_plane   = 100.0f;
int camera_ent_idx;

f32 editor_perspective = 45.0f;
f32 editor_near_plane = 0.1f;
f32 editor_far_plane = 100.0f;

// entities
struct { int key; entity value; }* entities = NULL;
// entity* entities = NULL;
int  entities_len = 0;
int* entity_ids = NULL;
int  entity_ids_len = 0;

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
vec3	 wireframe_color = { 0.0f, 0.0f, 0.0f };

#ifdef EDITOR_ACT
shader modes_shader;
#endif

// framebuffer
mesh m;
shader screen_shader;
u32 tex_aa_buffer;
u32 tex_col_buffer;
u32 quad_vao, quad_vbo;
bee_bool use_msaa = BEE_TRUE;

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
	screen_shader = add_shader("screen.vert", "screen.frag", "SHADER_framebuffer");

	create_framebuffer_multisampled(&tex_aa_buffer);
	create_framebuffer(&tex_col_buffer);
	set_framebuffer_to_update(&tex_aa_buffer);  // updates framebuffer on window resize
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
	skybox_shader = add_shader("cube_map.vert", "cube_map.frag", "SHADER_skybox");

	cube_map = load_cubemap("right.jpg", "left.jpg", "bottom.jpg", "top.jpg", "front.jpg", "back.jpg");

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
	modes_shader = add_shader("basic.vert", "modes.frag", "SHADER_modes");
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

	// set default return if key doesn't exist
	entity e; 
	e.has_trans = 0; e.has_model = 0; e.has_cam = 0;  e.has_light = 0;
	e.name = "x"; e.scripts_len = 0; e.children_len = 0; e.id = -1; e.id_idx = -1;
	hmdefault(entities, e);
}

void renderer_update()
{
	// first pass
	bind_framebuffer(use_msaa); // bind multisampled fbo or color texture fbo
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear bg
	
	// need to restore the opengl state after calling nuklear
	glEnable(GL_BLEND); // enable blending of transparent texture
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //set blending function: 1 - source_alpha, e.g. 0.6(60%) transparency -> 1 - 0.6 = 0.4(40%)
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
#ifdef EDITOR_ACT
	// this works when only activated once
	glEnable(GL_CULL_FACE); 
	glEnable(GL_DEPTH_TEST); // enable the z-buffer
#endif

	int entity_ids_len = 0;
	int* entity_ids = get_entity_ids(&entity_ids_len);

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
	for (int i = 0; i < entity_ids_len; ++i)
	{
		// skip transparent objects
		bee_bool is_trans = BEE_FALSE;
		for (int n = 0; n < transparent_ents_len; ++n)
		{
			if (entity_ids[i] == transparent_ents[n]) { is_trans = BEE_TRUE; break; }
		}
		if (is_trans) { continue; }

		entity* ent = get_entity(entity_ids[i]);
		if (ent->has_model)
		{
			vec3 pos   = { 0.0f, 0.0f, 0.0f };
			vec3 rot   = { 0.0f, 0.0f, 0.0f };
			vec3 scale = { 0.0f, 0.0f, 0.0f };
			get_entity_global_transform(entity_ids[i], pos, rot, scale);
			draw_mesh(&ent->_mesh, ent->_material, pos, rot, scale, ent->rotate_global); // entities[i].pos, entities[i].rot, entities[i].scalef
		}
		#ifdef EDITOR_ACT
		if (!(gamestate && hide_gizmos) && (ent->has_light || ent->has_cam))
		{
			vec3 pos = { 0.0f, 0.0f, 0.0f };
			vec3 rot = { 0.0f, 0.0f, 0.0f };
			vec3 scale = { 0.0f, 0.0f, 0.0f };
			get_entity_global_transform(entity_ids[i], pos, rot, scale);
			mesh* m = NULL;
			if (ent->has_cam) { m = get_mesh("camera.obj"); }
			else if (ent->has_light) 
			{
				switch (ent->_light.type)
				{
					case DIR_LIGHT:
						m = get_mesh("arrow_down.obj"); 
						break;
					case POINT_LIGHT:
						m = get_mesh("lightbulb.obj");
						break;
					case SPOT_LIGHT:
						m = get_mesh("flashlight.obj");
						break;
				}
			}
			if (m == NULL) { continue; }
			draw_mesh(m, get_material("MAT_cel"), pos, rot, scale, ent->rotate_global); // entities[i].pos, entities[i].rot, entities[i].scalef
		}
		#endif
	}

	// sort transparent / translucent objects
	vec3 cam_pos; get_editor_camera_pos(&cam_pos);
	for(int rpt = 0; rpt < BLEND_SORT_DEPTH; ++rpt)
	{
		// sort by distance
		// for (int i = 0; (i + 1) < transparent_ents_len; ++i)
		for (int i = transparent_ents_len -2; i > 0; --i)
		{
			// calc dist for each transparent obj
			f32 dist01 = glm_vec3_distance(cam_pos, get_entity(transparent_ents[i]   )->pos);
			f32 dist02 = glm_vec3_distance(cam_pos, get_entity(transparent_ents[i +1])->pos);
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
		if (gamestate)
		{
			get_camera_view_mat(get_entity(camera_ent_idx), &view[0]);
		}
		else 
		{
			get_editor_camera_view_mat(&view[0]);
		}
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
		entity* ent = get_entity(i);
		if (ent->has_model)
		{
			vec3 pos = { 0.0f, 0.0f, 0.0f };
			vec3 rot = { 0.0f, 0.0f, 0.0f };
			vec3 scale = { 0.0f, 0.0f, 0.0f };
			get_entity_global_transform(i, pos, rot, scale);
			draw_mesh(&ent->_mesh, ent->_material, pos, rot, scale, ent->rotate_global);
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
	if (use_msaa)
	{
		blit_multisampled_framebuffer(); // convert multisampled buffer to normal texture
	}
	unbind_framebuffer();

	// glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	glUseProgram(screen_shader.handle);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_col_buffer);
	shader_set_int(screen_shader, "tex", 0);
	glBindVertexArray(quad_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glEnable(GL_DEPTH_TEST);
	// ------------------------------------------------------------------------

	// entities ---------------------------------------------------------------

	for (int i = 0; i < entity_ids_len; ++i)
	{
		update_entity(get_entity(entity_ids[i]));
	}
	check_for_level_load(); // check if one of the run scripts requested to change scene

	// ------------------------------------------------------------------------

#ifdef EDITOR_ACT
	// do this so the nuklear gui is always drawn in solid-mode
	// enable solid-mode in case wireframe-mode is on
	if (wireframe_mode_enabled == BEE_TRUE)
	{ glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }
#endif
}

void draw_mesh(mesh* _mesh, material* mat, vec3 pos, vec3 rot, vec3 scale, bee_bool rotate_global)
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
	if (gamestate)
	{
		get_camera_view_mat(get_entity(camera_ent_idx), &view[0]);
	}
	else
	{
		get_editor_camera_view_mat(&view[0]);
	}
	

	mat4 proj;

	entity* cam = get_entity(camera_ent_idx);
	f32 deg_pers = cam->_camera.perspective;
	f32 near_p   = cam->_camera.near_plane;
	f32 far_p    = cam->_camera.far_plane;
#ifdef EDITOR_ACT
	if (!gamestate) // play-mode
	{
		deg_pers = perspective;
		near_p   = near_plane;
		far_p    = far_plane;
	}
#endif
	glm_make_rad(&deg_pers);
	int w, h; get_window_size(&w, &h);
	glm_perspective(deg_pers, ((float)w / (float)h), near_p, far_p, proj);

#ifdef EDITOR_ACT
	if (wireframe_mode_enabled == BEE_TRUE || normal_mode_enabled == BEE_TRUE || uv_mode_enabled == BEE_TRUE) 
	{
		// act the shader
		shader_use(modes_shader);

		// set shader matrices ------------------------------
		shader_set_mat4(modes_shader, "model", &model[0]);
		shader_set_mat4(modes_shader, "view", &view[0]);
		shader_set_mat4(modes_shader, "proj", &proj[0]);

		vec3 cam_pos; 
		if (gamestate)
		{
			glm_vec3_copy(get_entity(camera_ent_idx)->pos, cam_pos);
		}
		else
		{
			get_editor_camera_pos(cam_pos);
		}
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

		if (gamestate) // in play-mode
		{
			shader_set_vec3(mat->shader, "viewPos", get_entity(camera_ent_idx)->pos);
		}
		else 
		{
			vec3 cam_pos; get_editor_camera_pos(cam_pos);
			shader_set_vec3(mat->shader, "viewPos", cam_pos);
		}

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
		entity* light;
		vec3 pos_l   = { 0, 0, 0 };
		vec3 rot_l   = { 0, 0, 0 };
		vec3 scale_l = { 0, 0, 0 };
		shader_set_int(mat->shader, "Num_DirLights", dir_lights_len);
		int disabled_lights = 0;
		for (int i = 0; i < dir_lights_len; ++i)
		{
			light = get_entity(dir_lights[i]);
			if (!light->_light.enabled) 
			{ 
				disabled_lights++;
				shader_set_int(mat->shader, "Num_DirLights", dir_lights_len - disabled_lights); 
				continue; 
			}
			int idx = i - disabled_lights;
			sprintf(buffer, "dirLights[%d].direction", idx);
			shader_set_vec3(mat->shader, buffer, light->_light.direction);

			sprintf(buffer, "dirLights[%d].ambient", idx);
			shader_set_vec3(mat->shader, buffer, light->_light.ambient);
			sprintf(buffer, "dirLights[%d].diffuse", idx);
			shader_set_vec3(mat->shader, buffer, light->_light.diffuse);
			sprintf(buffer, "dirLights[%d].specular", idx);
			shader_set_vec3(mat->shader, buffer, light->_light.specular);
		}
		shader_set_int(mat->shader, "Num_PointLights", point_lights_len);
		disabled_lights = 0;
		for (int i = 0; i < point_lights_len; ++i)
		{
			light = get_entity(point_lights[i]);
			if (!light->_light.enabled) 
			{ 
				disabled_lights++; 
				shader_set_int(mat->shader, "Num_PointLights", point_lights_len - disabled_lights); 
				continue; 
			}
			int idx = i - disabled_lights;
			get_entity_global_transform(point_lights[i], pos_l, rot_l, scale_l);
			sprintf(buffer, "pointLights[%d].position", idx);
			shader_set_vec3(mat->shader, buffer, pos_l);

			sprintf(buffer, "pointLights[%d].ambient", idx);
			shader_set_vec3(mat->shader, buffer, light->_light.ambient);
			sprintf(buffer, "pointLights[%d].diffuse", idx);
			shader_set_vec3(mat->shader, buffer, light->_light.diffuse);
			sprintf(buffer, "pointLights[%d].specular", idx);
			shader_set_vec3(mat->shader, buffer, light->_light.specular);
			sprintf(buffer, "pointLights[%d].constant", idx);
			shader_set_float(mat->shader, buffer, light->_light.constant);
			sprintf(buffer, "pointLights[%d].linear", idx);
			shader_set_float(mat->shader, buffer, light->_light.linear);
			sprintf(buffer, "pointLights[%d].quadratic", idx);
			shader_set_float(mat->shader, buffer, light->_light.quadratic);
		}
		shader_set_int(mat->shader, "Num_SpotLights", spot_lights_len);
		disabled_lights = 0;
		for (int i = 0; i < spot_lights_len; ++i)
		{
			light = get_entity(spot_lights[i]);
			if (!light->_light.enabled) 
			{ 
				disabled_lights++;
				shader_set_int(mat->shader, "Num_SpotLights", spot_lights_len - disabled_lights); 
				continue; 
			}
			int idx = i - disabled_lights;
			get_entity_global_transform(spot_lights[i], pos_l, rot_l, scale_l);
			sprintf(buffer, "spotLights[%d].position", idx);
			shader_set_vec3(mat->shader, buffer, pos_l);

			sprintf(buffer, "spotLights[%d].direction", idx);
			shader_set_vec3(mat->shader, buffer, light->_light.direction);

			sprintf(buffer, "spotLights[%d].ambient", idx);
			shader_set_vec3(mat->shader, buffer, light->_light.ambient);
			sprintf(buffer, "spotLights[%d].diffuse", idx);
			shader_set_vec3(mat->shader, buffer, light->_light.diffuse);
			sprintf(buffer, "spotLights[%d].specular", idx);
			shader_set_vec3(mat->shader, buffer, light->_light.specular);
			sprintf(buffer, "spotLights[%d].constant", idx);
			shader_set_float(mat->shader, buffer, light->_light.constant);
			sprintf(buffer, "spotLights[%d].linear", idx);
			shader_set_float(mat->shader, buffer, light->_light.linear);
			sprintf(buffer, "spotLights[%d].quadratic", idx);
			shader_set_float(mat->shader, buffer, light->_light.quadratic);
			sprintf(buffer, "spotLights[%d].cutOff", idx);
			shader_set_float(mat->shader, buffer, light->_light.cut_off);
			sprintf(buffer, "spotLights[%d].outerCutOff", idx);
			shader_set_float(mat->shader, buffer, light->_light.outer_cut_off);
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

void get_entity_global_transform(int idx, vec3* pos, vec3* rot, vec3* scale)
{
	entity* ent = get_entity(idx);
	if (ent->has_trans && ent->parent != 9999 && (get_entity(ent->parent))->has_trans)
	{
		vec3 parent_pos   = { 0.0f, 0.0f, 0.0f };
		vec3 parent_rot   = { 0.0f, 0.0f, 0.0f };
		vec3 parent_scale = { 0.0f, 0.0f, 0.0f };
		get_entity_global_transform(ent->parent, parent_pos, parent_rot, parent_scale);

		glm_vec3_add(parent_pos, ent->pos, *pos);

		// need to rotate the object around its parents position by the parents rotation
		// glm_vec3_copy(ent->rot, *rot); // ??? lol
		glm_vec3_add(parent_rot, ent->rot, *rot); // ??? lol
		
		glm_vec3_mul(parent_scale, ent->scale, *scale);
	}
	else if (ent->has_trans)
	{
		// just use the entities transform
		glm_vec3_copy(ent->pos, *pos);
		glm_vec3_copy(ent->rot, *rot);
		glm_vec3_copy(ent->scale, *scale);
	}
	else 
	{
		// just use a default transform
		vec3 zero = { 0.0f, 0.0f, 0.0f };
		vec3 one  = { 1.0f, 1.0f, 1.0f };
		glm_vec3_copy(zero, *pos);
		glm_vec3_copy(zero, *rot);
		glm_vec3_copy(one,  *scale);
	}
}

void renderer_cleanup()
{
	for (int i = 0; i < entities_len; ++i)
	{
		free_entity(&entities[i]);
	}

	// arrfree(entities);
	hmfree(entities);
	arrfree(dir_lights);
	arrfree(point_lights);
	arrfree(spot_lights);
	arrfree(transparent_ents);
}

void renderer_clear_scene()
{
	// for (int i = 0; i < entity_ids_len; ++i)
	// {
	// 	hmdel(entities, entity_ids[i]);
	// }
	hmfree(entities);
	entities = NULL;
	entities_len = 0;
	// ------------------
	// for (int i = 0; i < entity_ids_len; ++i)
	// {
	// 	bee_bool success = arrdel(entity_ids, i);
	// 	printf("deleting entity - %s\n", success == 0 ? "FAILED" : "SUCCESS");
	// }
	arrfree(entity_ids);
	entity_ids = NULL;
	entity_ids_len = 0;
	// for (int i = transparent_ents_len - 1; i >= 0; --i)
	// {
	// 	arrdel(transparent_ents, i);
	// }
	arrfree(transparent_ents);
	transparent_ents = NULL;
	transparent_ents_len = 0;

	// for (int i = dir_lights_len - 1; i >= 0; --i)
	// {
	// 	arrdel(dir_lights, i);
	// }
	arrfree(dir_lights);
	dir_lights = NULL;
	dir_lights_len = 0;

	// for (int i = point_lights_len - 1; i >= 0; --i)
	// {
	// 	arrdel(point_lights, i);
	// }
	arrfree(point_lights);
	point_lights = NULL;
	point_lights_len = 0;

	// for (int i = spot_lights_len - 1; i >= 0; --i)
	// {
	// 	arrdel(spot_lights, i);
	// }
	arrfree(spot_lights);
	spot_lights = NULL;
	spot_lights_len = 0;
	
	camera_ent_idx = 0;
	editor_perspective = 45.0f;
	editor_near_plane = 0.1f;
	editor_far_plane = 100.0f;
}

// add an entity
int add_entity(vec3 pos, vec3 rot, vec3 scale, mesh* _mesh, material* _material, camera* _cam, light* _light, char* name)
{
	return add_entity_direct(make_entity(pos, rot, scale, _mesh, _material, _cam, _light, name));
}
int add_entity_direct(entity e) 
{
	// need to set any values before putting in hm
	e.id = entities_len;
	e.id_idx = entity_ids_len;

	if (e.has_model && e._material != NULL && e._material->is_transparent)
	{
		transparent_ents_len++;
		arrput(transparent_ents, entities_len);
	}

	if (e.has_light)
	{
		printf(" -> registered light id: %d, entity: %s\n", entities_len -1, e.name);
		switch (e._light.type)
		{
		case DIR_LIGHT:
			arrput(dir_lights, entities_len);
			e._light.id = dir_lights_len;
			dir_lights_len++;
		case POINT_LIGHT:
			arrput(point_lights, entities_len);
			e._light.id = point_lights_len;
			point_lights_len++;
		case SPOT_LIGHT:
			arrput(spot_lights, entities_len);
			e._light.id = spot_lights_len;
			spot_lights_len++;
		}
	}

	if (e.has_cam)
	{
		camera_ent_idx = entities_len;
	}

	hmput(entities, entities_len, e);
	printf("added entity: \"%s\", idx: %d\n", e.name, entities_len);
	arrput(entity_ids, entities_len);
	entities_len++;
	entity_ids_len++;

	return entities_len - 1;
}
int duplicate_entity(int id)
{
	entity e;
	entity* original = get_entity(id);
	// memcpy(&e, original, sizeof(entity));
	e.id     = -1;
	e.id_idx = -1;
	char* name = malloc(strlen(original->name) +2 +1);
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

	// @TODO: scripts prob should be copied as well

	return add_entity_direct(e);
}
void add_entity_cube()
{
	mesh m = make_cube_mesh(); // @TODO: make this check with the assetmanager if a cube mesh already exists
	vec3 pos = { 0.0f, 0.0f, 0.0f };
	vec3 rot = { 0.0f, 0.0f, 0.0f };
	vec3 scale = { 1.0f, 1.0f, 1.0f };
	add_entity(pos, rot, scale, &m, get_material("MAT_blank"), NULL, NULL, "cube");
}

// doesnt work yet
void entity_switch_light_type(int entity_id, light_type new_type)
{
	entity* ent = get_entity(entity_id);
	if (!ent->has_light) { return; }
	// remove from old type categorisation
	switch (ent->_light.type)
	{
		case DIR_LIGHT:
			arrdel(dir_lights, ent->_light.id);
			dir_lights_len--;
		case POINT_LIGHT:
			arrdel(point_lights, ent->_light.id);
			point_lights_len--;
		case SPOT_LIGHT:
			arrdel(spot_lights, ent->_light.id);
			spot_lights_len--;
	}

	// add to new type categorisation
	switch (new_type)
	{
		case DIR_LIGHT:
			arrput(dir_lights, entity_id);
			dir_lights_len++;
		case POINT_LIGHT:
			arrput(point_lights, entity_id);
			point_lights_len++;
		case SPOT_LIGHT:
			arrput(spot_lights, entity_id);
			spot_lights_len++;
	}

	ent->_light.type = new_type;
}

void entity_add_script(int entity_index, const char* name)
{
	entity* ent = &hmget(entities, entity_index);
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

#ifdef EDITOR_ACT
void set_gamestate(bee_bool play, bee_bool _hide_gizmos)
{
	if (play == BEE_SWITCH)
	{
		set_gamestate(!gamestate, _hide_gizmos);
		return;
	}

	gamestate = play;
	hide_gizmos = _hide_gizmos;

	if (play)
	{
		save_scene_state();
		// set in-game cam to be act
		entity* ent_cam = get_entity(camera_ent_idx);
		if (ent_cam == NULL || ent_cam->has_cam == BEE_FALSE) 
		{ 
			printf("[ERROR] No Camera in Scene.\n");  
			submit_txt_console("[ERROR] No Camera in Scene.");
			set_error_popup(GENERAL_ERROR, "[ERROR] No Camera in Scene.");
			gamestate = BEE_FALSE;
			return;
		}
		// perspective = ent_cam->_camera.perspective;
		// near_plane  = ent_cam->_camera.near_plane;
		// far_plane   = ent_cam->_camera.far_plane;
	}
	else
	{
		load_scene_state();
		set_cursor_visible(BEE_TRUE);
		// act editor cam
		perspective = editor_perspective;
		near_plane  = editor_near_plane;
		far_plane   = editor_far_plane;
	}

	set_all_scripts(play);
	// if (hide_gizmos)
	// {
	// 	set_all_gizmo_meshes(!play);
	// }
}
bee_bool get_gamestate()
{
	return gamestate;
}
void set_all_scripts(bee_bool act)
{
	for (int i = 0; i < entities_len; ++i)
	{
		entity* ent = get_entity(i);
		for (int n = 0; n < ent->scripts_len; ++n)
		{
			ent->scripts[n]->active = act;
		}
	}
}
#endif
void renderer_set(render_setting setting, bee_bool value)
{
	if (value == BEE_SWITCH)
	{
		renderer_set(setting, !renderer_get(setting));
		return;
	}
	switch (setting)
	{
		case RENDER_WIREFRAME:
			wireframe_mode_enabled = value;
			break;
		case RENDER_UV:
			uv_mode_enabled = value;
			break;
		case RENDER_NORMAL:
			normal_mode_enabled = value;
			break;
		case RENDER_CUBEMAP:
			draw_skybox = value;
			break;
		case RENDER_MSAA:
			use_msaa = value;
			break;
	}
}
bee_bool* renderer_get(render_setting setting)
{
	switch (setting)
	{
		case RENDER_WIREFRAME:
			return &wireframe_mode_enabled;
		case RENDER_UV:
			return &uv_mode_enabled;
		case RENDER_NORMAL:
			return &normal_mode_enabled;
		case RENDER_CUBEMAP:
			return &draw_skybox;
		case RENDER_MSAA:
			return &use_msaa;
	}
}

void entity_set_parent(int child, int parent)
{
	if (child == parent) { return; }

	entity* parent_ent = get_entity(parent);
	entity* child_ent  = get_entity(child);
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
	glm_vec3_copy(pos,   child_ent->pos);
	glm_vec3_copy(rot,   child_ent->rot);
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


// doesnt work yet
void entity_remove(int entity_idx)
{
	entity* ent = get_entity(entity_idx);
	if (ent->has_light)
	{
		switch (ent->_light.type)
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
	return get_entity(camera_ent_idx);
}
int get_entity_id_by_name(char* name)
{
	for (int i = 0; i < entities_len; ++i)
	{
		if (strcmp(get_entity(entity_ids[i])->name, name) == 0)
		{
			return i;
		}
	}
	assert(0); // no entity with given name
	return 9999;
}


#ifdef EDITOR_ACT
renderer_properties get_renderer_properties()
{
	renderer_properties prop;
	prop.perspective = &editor_perspective;
	prop.near_plane  = &editor_near_plane;
	prop.far_plane   = &editor_far_plane;

	prop.wireframe_col_r = &wireframe_color[0];
	prop.wireframe_col_g = &wireframe_color[1];
	prop.wireframe_col_b = &wireframe_color[2];

	return prop;
}
#endif