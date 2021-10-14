#include "renderer.h"

#define STB_DS_IMPLEMENTATION
#include "stb/stb_ds.h"
#include "stb/stb_image.h"
#include "GLAD/glad.h"

#include "input.h"
#include "shader.h"
#include "window.h"
#include "camera.h"
#include "entities.h"
#include "editor_ui.h"
#include "framebuffer.h"
#include "file_handler.h"
#include "asset_manager.h"
#include "scene_manager.h"
#include "gravity_interface.h"


#define F32_PER_VERT 11
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
int camera_ent_idx = -1; // -1 = no camera

f32 editor_perspective = 45.0f;
f32 editor_near_plane = 0.1f;
f32 editor_far_plane = 100.0f;

// hdr
f32 exposure = 1.0f;

vec3 bg_color = { 0.1f, 0.1f, 0.1f };

#ifdef EDITOR_ACT
int draw_calls_per_frame = 0;
int verts_per_frame		= 0;
int tris_per_frame		= 0;
#endif

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
shader* modes_shader;
#endif

// post processing
shader* screen_shader;
u32 quad_vao, quad_vbo;

// skybox
u32 cube_map;
shader* skybox_shader;
u32 skybox_vao, skybox_vbo;
bee_bool draw_skybox = BEE_TRUE;

// msaa & color buffer
u32 tex_aa_buffer;
u32 tex_aa_fbo;
u32 tex_aa_rbo;
u32 tex_col_buffer;
u32 tex_col_fbo;
u32 tex_col_rbo;
bee_bool use_msaa = BEE_TRUE;

// shadow mapping
shader* shadow_shader;
const f32 shadow_near_plane = 1.0f, shadow_far_plane = 7.5f;

#ifdef EDITOR_ACT
// mouse picking
u32 mouse_pick_buffer;
u32 mouse_pick_fbo;
u32 mouse_pick_rbo;
shader* mouse_pick_shader;
#endif


void renderer_init()
{
	// framebuffer ----------------------------------------------------------------------------------
	
	screen_shader = add_shader("screen.vert", "screen.frag", "SHADER_framebuffer");
				  
	shadow_shader = add_shader("shadow_map.vert", "empty.frag", "SHADER_shadow");
	

	create_framebuffer_hdr(&tex_col_buffer, &tex_col_fbo, &tex_col_rbo);
	create_framebuffer_multisampled_hdr(&tex_aa_buffer, &tex_aa_fbo, &tex_aa_rbo);
	set_framebuffer_to_update(tex_col_buffer); // updates framebuffer on window resize
	set_framebuffer_to_update(tex_aa_buffer);  // updates framebuffer on window resize

#ifdef EDITOR_ACT
	mouse_pick_shader = add_shader("basic.vert", "mouse_picking.frag", "SHADER_mouse_pick");
	create_framebuffer_mouse_picking(&mouse_pick_buffer, &mouse_pick_fbo, &mouse_pick_rbo);
	set_framebuffer_to_update(mouse_pick_buffer);  // updates framebuffer on window resize
#endif

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
	set_bg_color(bg_color);


	// light* l = &get_entity(dir_lights[0])->_light;
	// create_framebuffer_shadowmap(&l->shadow_map, &l->shadow_fbo, SHADOW_MAP_SIZE_X, SHADOW_MAP_SIZE_Y);
}

void renderer_update()
{
#ifdef EDITOR_ACT
	draw_calls_per_frame = 0;
	verts_per_frame		= 0;
	tris_per_frame		= 0;
#endif

#ifdef EDITOR_ACT
	render_scene_mouse_pick();
#endif
	render_scene_shadows();
	render_scene_normal();
	render_scene_screen();

#ifdef EDITOR_ACT
	// do this so the nuklear gui is always drawn in solid-mode
	// enable solid-mode in case wireframe-mode is on
	if (wireframe_mode_enabled == BEE_TRUE)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
#endif

}

#ifdef EDITOR_ACT
void render_scene_mouse_pick()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	int w, h; get_window_size(&w, &h);
	glViewport(0, 0, w / 4, h / 4);
	bind_framebuffer(mouse_pick_fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear bg

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
	f32 near_p = cam->_camera.near_plane;
	f32 far_p = cam->_camera.far_plane;
#ifdef EDITOR_ACT
	if (!gamestate) // play-mode
	{
		deg_pers = perspective;
		near_p = near_plane;
		far_p = far_plane;
	}
#endif
	glm_make_rad(&deg_pers);
	glm_perspective(deg_pers, ((float)w / (float)h), near_p, far_p, proj);

	// cycle all objects
	int entity_ids_len = 0;
	int* entity_ids = get_entity_ids(&entity_ids_len);
	for (int i = 0; i < entity_ids_len; ++i)
	{
		entity* ent = get_entity(entity_ids[i]);
		if (ent->has_model && ent->_mesh.visible)
		{
			// MVP for mesh
			vec3 pos = { 0.0f, 0.0f, 0.0f };
			vec3 rot = { 0.0f, 0.0f, 0.0f };
			vec3 scale = { 0.0f, 0.0f, 0.0f };
			get_entity_global_transform(entity_ids[i], pos, rot, scale);

			mat4 model = GLM_MAT4_IDENTITY_INIT;
			f32 x = rot[0];  glm_make_rad(&x);
			f32 y = rot[1];  glm_make_rad(&y);
			f32 z = rot[2];  glm_make_rad(&z);

			const vec3 x_axis = { 1.0f, 0.0f, 0.0f };
			const vec3 y_axis = { 0.0f, 1.0f, 0.0f };
			const vec3 z_axis = { 0.0f, 0.0f, 1.0f };
			glm_rotate_at(model, pos, x, x_axis);
			glm_rotate_at(model, pos, y, y_axis);
			glm_rotate_at(model, pos, z, z_axis);
			glm_translate(model, pos);
			glm_scale(model, scale);

			shader_use(mouse_pick_shader);
			shader_set_mat4(mouse_pick_shader, "model", model);
			shader_set_mat4(mouse_pick_shader, "view", view);
			shader_set_mat4(mouse_pick_shader, "proj", proj);

			shader_set_float(mouse_pick_shader, "id", (f32)ent->id);

			glBindVertexArray(ent->_mesh.vao);
			if (ent->_mesh.indexed == BEE_TRUE)
			{
				glDrawElements(GL_TRIANGLES, (ent->_mesh.indices_len), GL_UNSIGNED_INT, 0);
			}
			else
			{
				glDrawArrays(GL_TRIANGLES, 0, (ent->_mesh.vertices_len / F32_PER_VERT));
			}

		}
	}
	unbind_framebuffer();

	// reset bg color
	vec3 col; 
	get_bg_color(col);
	set_bg_color(col);
}
#endif

void render_scene_shadows()
{
	glEnable(GL_DEPTH_TEST);
	// glEnable(GL_CULL_FACE);
	// glCullFace(GL_FRONT);

	mat4 proj; //  = GLM_MAT4_IDENTITY_INIT
	glm_ortho(-10.0f, 10.0f, -10.0f, 10.0f, shadow_near_plane, shadow_far_plane, proj);

	// mat4 view; //  = GLM_MAT4_IDENTITY_INIT
	// vec3 eye = { -2.0f, 4.0f, -1.0f };
	vec3 center = { 0.0f,  0.0f,  0.0f };
	vec3 up = { 0.0f,  1.0f,  0.0f };
	// glm_lookat(eye, center, up, view);

	for (int n = 0; n < dir_lights_len; ++n)
	{
		entity* l = get_entity(dir_lights[n]);
		if (!l->_light.cast_shadow)
		{
			continue;
		}
		glViewport(0, 0, l->_light.shadow_map_x, l->_light.shadow_map_y);
		bind_framebuffer(l->_light.shadow_fbo);
		glClear(GL_DEPTH_BUFFER_BIT);

		mat4 view;
		glm_vec3_add(l->pos, l->_light.direction, center);
		glm_lookat(l->pos, center, up, view);

		mat4 light_space;
		glm_mat4_mul(proj, view, light_space);
		glm_mat4_copy(light_space, l->_light.light_space);

		// cycle all objects
		int entity_ids_len = 0;
		int* entity_ids = get_entity_ids(&entity_ids_len);
		for (int i = 0; i < entity_ids_len; ++i)
		{
			entity* ent = get_entity(entity_ids[i]);
			if (ent->has_model && ent->_mesh.visible)
			{
				// MVP for mesh
				vec3 pos = { 0.0f, 0.0f, 0.0f };
				vec3 rot = { 0.0f, 0.0f, 0.0f };
				vec3 scale = { 0.0f, 0.0f, 0.0f };
				get_entity_global_transform(entity_ids[i], pos, rot, scale);

				mat4 model = GLM_MAT4_IDENTITY_INIT;
				f32 x = rot[0];  glm_make_rad(&x);
				f32 y = rot[1];  glm_make_rad(&y);
				f32 z = rot[2];  glm_make_rad(&z);

				const vec3 x_axis = { 1.0f, 0.0f, 0.0f };
				const vec3 y_axis = { 0.0f, 1.0f, 0.0f };
				const vec3 z_axis = { 0.0f, 0.0f, 1.0f };
				glm_rotate_at(model, pos, x, x_axis);
				glm_rotate_at(model, pos, y, y_axis);
				glm_rotate_at(model, pos, z, z_axis);
				glm_translate(model, pos);
				glm_scale(model, scale);

				shader_use(shadow_shader);
				shader_set_mat4(shadow_shader, "model", model);
				// shader_set_mat4(shadow_shader, "view", view);
				// shader_set_mat4(shadow_shader, "proj", proj);
				shader_set_mat4(shadow_shader, "light_space", light_space);

				glBindVertexArray(ent->_mesh.vao);
				if (ent->_mesh.indexed == BEE_TRUE)
				{
					glDrawElements(GL_TRIANGLES, (ent->_mesh.indices_len), GL_UNSIGNED_INT, 0);
				}
				else
				{
					glDrawArrays(GL_TRIANGLES, 0, (ent->_mesh.vertices_len / F32_PER_VERT));
				}

			}
		}

		unbind_framebuffer();
	}


	// glCullFace(GL_BACK);
}

void render_scene_normal()
{

	// first pass
	int w, h;
	get_window_size(&w, &h);
	glViewport(0, 0, w, h);
	bind_framebuffer(use_msaa ? tex_aa_fbo : tex_col_fbo); // bind multisampled fbo or color texture fbo
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear bg
	
	// need to restore the opengl state after calling nuklear
	glEnable(GL_BLEND); // enable blending of transparent texture
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //set blending function: 1 - source_alpha, e.g. 0.6(60%) transparency -> 1 - 0.6 = 0.4(40%)
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
#ifdef EDITOR_ACT
	// this works when only activated once
	glEnable(GL_CULL_FACE); 
	glEnable(GL_DEPTH_TEST); // enable the z-buffer

	// // object outline
	// glClear(GL_STENCIL_BUFFER_BIT);
	// glEnable(GL_STENCIL_TEST);
	// glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	// glStencilFunc(GL_ALWAYS, 1, 0xFF); // let all frags pass
	// glStencilMask(0x00); // disable writing to the stencil mask
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
	int entity_ids_len = 0;
	int* entity_ids = get_entity_ids(&entity_ids_len);
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
			// if (entity_ids[i] == get_selected_entity())
			// {
			// 	glStencilMask(0xFF); // enable writing to mask
			// }
			vec3 pos   = { 0.0f, 0.0f, 0.0f };
			vec3 rot   = { 0.0f, 0.0f, 0.0f };
			vec3 scale = { 0.0f, 0.0f, 0.0f };
			get_entity_global_transform(entity_ids[i], pos, rot, scale); vec3 v = GLM_VEC2_ZERO_INIT;
			draw_mesh(&ent->_mesh, ent->_material, pos, rot, scale, ent->rotate_global, BEE_FALSE, v); // entities[i].pos, entities[i].rot, entities[i].scalef
			
			// if (entity_ids[i] == get_selected_entity())
			// {
			// 	glStencilMask(0x00); // disable writing to mask
			// }
		}
		#ifdef EDITOR_ACT
		if (!(gamestate && hide_gizmos) && (ent->has_light || ent->has_cam))
		{
			vec3 pos = { 0.0f, 0.0f, 0.0f };
			vec3 rot = { 0.0f, 0.0f, 0.0f };
			vec3 scale = { 0.0f, 0.0f, 0.0f };
			get_entity_global_transform(entity_ids[i], pos, rot, scale);
			mesh* m = NULL;
			vec3 tint = GLM_VEC3_ONE_INIT;
			if (ent->has_cam) 
			{
				m = get_mesh("camera.obj");
			}
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
				glm_vec3_copy(ent->_light.diffuse, tint);
				// printf("tint: %f, %f, %f\n", tint[0], tint[1], tint[2]);
				// printf("dif:  %f, %f, %f\n", ent->_light.diffuse[0], ent->_light.diffuse[1], ent->_light.diffuse[2]);
			}
			if (m == NULL) { continue; }
			material* mat = get_material("MAT_cel");
			draw_mesh(m, mat, pos, rot, scale, ent->rotate_global, BEE_TRUE, tint); // entities[i].pos, entities[i].rot, entities[i].scalef
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

#ifdef EDITOR_ACT
	// draw object outline ----------------------------------------------------
	if (get_selected_entity() != -1)
	{
		// glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		// glStencilMask(0x00); // disable writing to mask
		// glDisable(GL_DEPTH_TEST);
		// 
		// entity* ent = get_entity(get_selected_entity());
		// vec3 pos = { 0.0f, 0.0f, 0.0f };
		// vec3 rot = { 0.0f, 0.0f, 0.0f };
		// vec3 scale = { 0.0f, 0.0f, 0.0f };
		// get_entity_global_transform(get_selected_entity(), pos, rot, scale);
		// vec3 scalar = { 1.2f, 1.2f, 1.2f };
		// glm_vec3_mul(scale, scalar, scale);
		// // vec3 pos02 = { pos[0], pos[1] - (scale[1] * 0.8f), pos[2] };
		// // vec3 cam; get_editor_camera_pos(cam);  // cam pos
		// // vec3 dir; glm_vec3_sub(cam, pos, dir); // dir from obj to cam
		// // glm_vec3_normalize(dir);
		// // vec3 dist = { 1.2f, 1.2f, 1.2f };
		// // glm_vec3_mul(dir, dist, dir);
		// // glm_vec3_add(pos, dir, pos);
		// vec3 tint = { 11.0f / 255.0f, 1.0, 249.0f / 255.0f };
		// material* mat = get_material("MAT_cel");
		// draw_mesh(&ent->_mesh, mat, pos, rot, scale, ent->rotate_global, BEE_FALSE, tint);
		// 
		// glEnable(GL_DEPTH_TEST); // enable the z-buffer
	}
	// ------------------------------------------------------------------------
#endif

	// skybox
	if (draw_skybox == BEE_TRUE)
	{
		render_scene_skybox();
	}

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
			get_entity_global_transform(i, pos, rot, scale); vec3 v = GLM_VEC2_ZERO_INIT;
			draw_mesh(&ent->_mesh, ent->_material, pos, rot, scale, ent->rotate_global, BEE_FALSE, v);
		}
	}
	// ------------------------------------------------------------------------

	// prepare for second pass
	if (use_msaa)
	{
		blit_multisampled_framebuffer(tex_aa_fbo, tex_col_fbo); // convert multisampled buffer to normal texture
	}
	unbind_framebuffer();

}

void render_scene_skybox()
{
	// skybox -----------------------------------------------------------------
#ifdef EDITOR_ACT
	// draw in solid-mode for skybox
	if (wireframe_mode_enabled == BEE_TRUE)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
#endif
	// draw skybox as last
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	//shader_use(skybox_shader);
	shader_use(skybox_shader);

	mat4 view;
#ifdef EDITOR_ACT
	if (gamestate)
	{
		get_camera_view_mat(get_entity(camera_ent_idx), view);
	}
	else
	{
		get_editor_camera_view_mat(view);
	}
#else
	get_camera_view_mat(get_entity(camera_ent_idx), view);
#endif
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
	
	// ------------------------------------------------------------------------
}

void render_scene_screen()
{
	// framebuffer ------------------------------------------------------------
#ifdef EDITOR_ACT
	// draw in solid-mode for fbo
	if (wireframe_mode_enabled == BEE_TRUE)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
#endif

	// glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	shader_use(screen_shader);
	shader_set_float(screen_shader, "exposure", exposure);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_col_buffer); // wireframe_mode_enabled ? mouse_pick_buffer : 
	shader_set_int(screen_shader, "tex", 0);
	glBindVertexArray(quad_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glEnable(GL_DEPTH_TEST);
	// ------------------------------------------------------------------------
}

void draw_mesh(mesh* _mesh, material* mat, vec3 pos, vec3 rot, vec3 scale, bee_bool rotate_global, bee_bool is_gizmo, vec3 gizmo_col)
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
		shader_set_vec3(modes_shader, "view_pos", cam_pos);


		if (wireframe_mode_enabled == BEE_TRUE)
		{
			shader_set_int(modes_shader, "mode", 0);
			shader_set_vec3(modes_shader, "wiref_col", wireframe_color);
		}
		else if (normal_mode_enabled == BEE_TRUE)
		{
			shader_set_int(&modes_shader, "mode", 1);
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
		shader_set_mat4(mat->shader, "model", model);
		shader_set_mat4(mat->shader, "view", view);
		shader_set_mat4(mat->shader, "proj", proj);

		set_shader_uniforms(mat);

#ifdef EDITOR_ACT
		if (is_gizmo)
		{
			shader_set_vec3(mat->shader, "mat.tint", gizmo_col); // material because using cel_shader
		}
	}
#endif

	glBindVertexArray(_mesh->vao); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
	if (_mesh->indexed == BEE_TRUE)
	{
		glDrawElements(GL_TRIANGLES, (_mesh->indices_len), GL_UNSIGNED_INT, 0);
	}
	else
	{
		glDrawArrays(GL_TRIANGLES, 0, (_mesh->vertices_len / F32_PER_VERT)); // each vertices consist of 8 floats
	}

	if (mat->draw_backfaces == BEE_TRUE)
	{
		glEnable(GL_CULL_FACE);
	}
#ifdef EDITOR_ACT
	if (!is_gizmo)
	{
		draw_calls_per_frame++;
		verts_per_frame += _mesh->vertices_elems;
		tris_per_frame  += _mesh->indices_elems;
	}
#endif
}

void set_shader_uniforms(material* mat)
{
	if (gamestate) // in play-mode
	{
		shader_set_vec3(mat->shader, "view_pos", get_entity(camera_ent_idx)->pos);
	}
	else
	{
		vec3 cam_pos; get_editor_camera_pos(cam_pos);
		shader_set_vec3(mat->shader, "view_pos", cam_pos);
	}

	int texture_index = 0;

	// set shader light ---------------------------------
	if (mat->shader->use_lighting)
	{
		char buffer[28]; // pointLights[i].quadratic is the longest str at 24
		entity* light;
		vec3 pos_l = { 0, 0, 0 };
		vec3 rot_l = { 0, 0, 0 };
		vec3 scale_l = { 0, 0, 0 };
		shader_set_int(mat->shader, "num_dir_lights", dir_lights_len);
		int disabled_lights = 0;
		for (int i = 0; i < dir_lights_len; ++i)
		{
			light = get_entity(dir_lights[i]);
			if (!light->_light.enabled)
			{
				disabled_lights++;
				shader_set_int(mat->shader, "num_dir_lights", dir_lights_len - disabled_lights);
				continue;
			}
			int idx = i - disabled_lights;
			sprintf(buffer, "dir_lights[%d].direction", idx);
			shader_set_vec3(mat->shader, buffer, light->_light.direction);

			vec3 dif;
			vec3 intensity = { light->_light.dif_intensity, light->_light.dif_intensity , light->_light.dif_intensity };
			glm_vec3_mul(light->_light.diffuse, intensity, dif);

			sprintf(buffer, "dir_lights[%d].ambient", idx);
			shader_set_vec3(mat->shader, buffer, light->_light.ambient);
			sprintf(buffer, "dir_lights[%d].diffuse", idx);
			shader_set_vec3(mat->shader, buffer, dif);
			sprintf(buffer, "dir_lights[%d].specular", idx);
			shader_set_vec3(mat->shader, buffer, light->_light.specular);

			sprintf(buffer, "dir_lights[%d].use_shadow", idx);
			shader_set_int(mat->shader, buffer, light->_light.cast_shadow);
			sprintf(buffer, "dir_lights[%d].shadow_map", idx);
			glActiveTexture(GL_TEXTURE0 + texture_index);
			glBindTexture(GL_TEXTURE_2D, light->_light.shadow_map);
			shader_set_int(mat->shader, buffer, texture_index);
			texture_index++;
			sprintf(buffer, "dir_lights[%d].light_space", idx);
			shader_set_mat4(mat->shader, buffer, light->_light.light_space);

		}
		shader_set_int(mat->shader, "num_point_lights", point_lights_len);
		disabled_lights = 0;
		for (int i = 0; i < point_lights_len; ++i)
		{
			light = get_entity(point_lights[i]);
			if (!light->_light.enabled)
			{
				disabled_lights++;
				shader_set_int(mat->shader, "num_point_lights", point_lights_len - disabled_lights);
				continue;
			}
			int idx = i - disabled_lights;
			get_entity_global_transform(point_lights[i], pos_l, rot_l, scale_l);
			sprintf(buffer, "point_lights[%d].position", idx);
			shader_set_vec3(mat->shader, buffer, pos_l);

			vec3 dif;
			vec3 intensity = { light->_light.dif_intensity, light->_light.dif_intensity , light->_light.dif_intensity };
			glm_vec3_mul(light->_light.diffuse, intensity, dif);

			sprintf(buffer, "point_lights[%d].ambient", idx);
			shader_set_vec3(mat->shader, buffer, light->_light.ambient);
			sprintf(buffer, "point_lights[%d].diffuse", idx);
			shader_set_vec3(mat->shader, buffer, dif);
			sprintf(buffer, "point_lights[%d].specular", idx);
			shader_set_vec3(mat->shader, buffer, light->_light.specular);
			sprintf(buffer, "point_lights[%d].constant", idx);
			shader_set_float(mat->shader, buffer, light->_light.constant);
			sprintf(buffer, "point_lights[%d].linear", idx);
			shader_set_float(mat->shader, buffer, light->_light.linear);
			sprintf(buffer, "point_lights[%d].quadratic", idx);
			shader_set_float(mat->shader, buffer, light->_light.quadratic);
		}
		shader_set_int(mat->shader, "num_spot_lights", spot_lights_len);
		disabled_lights = 0;
		for (int i = 0; i < spot_lights_len; ++i)
		{
			light = get_entity(spot_lights[i]);
			if (!light->_light.enabled)
			{
				disabled_lights++;
				shader_set_int(mat->shader, "num_spot_lights", spot_lights_len - disabled_lights);
				continue;
			}
			int idx = i - disabled_lights;
			get_entity_global_transform(spot_lights[i], pos_l, rot_l, scale_l);
			sprintf(buffer, "spot_lights[%d].position", idx);
			shader_set_vec3(mat->shader, buffer, pos_l);

			sprintf(buffer, "spot_lights[%d].direction", idx);
			shader_set_vec3(mat->shader, buffer, light->_light.direction);

			vec3 dif;
			vec3 intensity = { light->_light.dif_intensity, light->_light.dif_intensity , light->_light.dif_intensity };
			glm_vec3_mul(light->_light.diffuse, intensity, dif);

			sprintf(buffer, "spot_lights[%d].ambient", idx);
			shader_set_vec3(mat->shader, buffer, light->_light.ambient);
			sprintf(buffer, "spot_lights[%d].diffuse", idx);
			shader_set_vec3(mat->shader, buffer, dif);
			sprintf(buffer, "spot_lights[%d].specular", idx);
			shader_set_vec3(mat->shader, buffer, light->_light.specular);
			sprintf(buffer, "spot_lights[%d].constant", idx);
			shader_set_float(mat->shader, buffer, light->_light.constant);
			sprintf(buffer, "spot_lights[%d].linear", idx);
			shader_set_float(mat->shader, buffer, light->_light.linear);
			sprintf(buffer, "spot_lights[%d].quadratic", idx);
			shader_set_float(mat->shader, buffer, light->_light.quadratic);
			sprintf(buffer, "spot_lights[%d].cutOff", idx);
			shader_set_float(mat->shader, buffer, light->_light.cut_off);
			sprintf(buffer, "spot_lights[%d].outerCutOff", idx);
			shader_set_float(mat->shader, buffer, light->_light.outer_cut_off);
		}
	}

	// set shader material ------------------------------
	if (mat->shader->use_lighting)
	{
		glActiveTexture(GL_TEXTURE0 + texture_index);
		glBindTexture(GL_TEXTURE_2D, mat->dif_tex.handle);
		shader_set_int(mat->shader, "mat.diffuse", texture_index);
		texture_index++;
		glActiveTexture(GL_TEXTURE0 + texture_index);
		glBindTexture(GL_TEXTURE_2D, mat->spec_tex.handle);
		shader_set_int(mat->shader, "mat.specular", texture_index);
		texture_index++;
		glActiveTexture(GL_TEXTURE0 + texture_index);
		glBindTexture(GL_TEXTURE_2D, mat->norm_tex.handle);
		shader_set_int(mat->shader, "mat.normal", texture_index);
		texture_index++;

		shader_set_float(mat->shader, "mat.shininess", mat->shininess);
		shader_set_vec2(mat->shader, "mat.tile", mat->tile);

		shader_set_vec3(mat->shader, "mat.tint", mat->tint);
	}

	// set shader uniforms ------------------------------
	for (int i = 0; i < mat->uniforms_len; ++i)
	{
		switch (mat->uniforms[i].def->type)
		{
			case UNIFORM_INT:
				shader_set_int(mat->shader, mat->uniforms[i].def->name, mat->uniforms[i].int_val);
				break;
			case UNIFORM_F32:
				shader_set_float(mat->shader, mat->uniforms[i].def->name, mat->uniforms[i].f32_val);
				break;
			case UNIFORM_VEC2:
				shader_set_vec2(mat->shader, mat->uniforms[i].def->name, mat->uniforms[i].vec2_val);
				break;
			case UNIFORM_VEC3:
				shader_set_vec3(mat->shader, mat->uniforms[i].def->name, mat->uniforms[i].vec3_val);
				break;
			case UNIFORM_TEX:
				glActiveTexture(GL_TEXTURE0 + texture_index);
				glBindTexture(GL_TEXTURE_2D, mat->uniforms[i].tex_val.handle);
				shader_set_int(mat->shader, mat->uniforms[i].def->name, texture_index);
				texture_index++;
				break;
		}
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
	arrfree(dir_lights);
	arrfree(point_lights);
	arrfree(spot_lights);
	arrfree(transparent_ents);
}

void renderer_clear_scene()
{
	
	arrfree(transparent_ents);
	transparent_ents = NULL;
	transparent_ents_len = 0;

	arrfree(dir_lights);
	dir_lights = NULL;
	dir_lights_len = 0;

	arrfree(point_lights);
	point_lights = NULL;
	point_lights_len = 0;

	arrfree(spot_lights);
	spot_lights = NULL;
	spot_lights_len = 0;
	
	camera_ent_idx = -1;
	editor_perspective = 45.0f;
	editor_near_plane = 0.1f;
	editor_far_plane = 100.0f;
}

#ifdef EDITOR_ACT
int read_mouse_position_mouse_pick_buffer_color()
{
	f64 x = 0;
	f64 y = 0;
	get_mouse_pos(&x, &y);
	int w = 0;
	int h = 0;
	get_window_size(&w, &h);
	y = h - y; // invert as buffer is rendered upside down
	// printf("mouse pos  %.2f, %.2f\n", x, y);
	x *= 0.25;
	y *= 0.25;
	// printf("buffer pos %d, %d\n", (int)x, (int)y);

	// int start_x = (int)(x * w);
	// int start_y = (int)(y * h);
	// printf("start pos %d, %d\n", start_x, start_y);
	f32 pixel[1];

	glGetError(); // clear any previous errors
	glBindFramebuffer(GL_READ_FRAMEBUFFER, mouse_pick_fbo);
	glReadPixels((int)x, (int)y, 1, 1, GL_RED, GL_FLOAT, pixel); // GL_UNSIGNED_BYTE
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	
	int error = glGetError();
	if (error != GL_NO_ERROR)
	{
		if (error == GL_INVALID_ENUM)				   { printf("[OpenGL ERROR] Invalid Enum\n"); }
		if (error == GL_INVALID_VALUE)				   { printf("[OpenGL ERROR] Invalid Value\n"); }
		if (error == GL_INVALID_OPERATION)			   { printf("[OpenGL ERROR] Invalid Operation\n"); }
		if (error == GL_STACK_OVERFLOW)				   { printf("[OpenGL ERROR] Stack Overflow\n"); }
		if (error == GL_STACK_UNDERFLOW)			   { printf("[OpenGL ERROR] Stack Underflow\n"); }
		if (error == GL_OUT_OF_MEMORY)				   { printf("[OpenGL ERROR] Out of Memory\n"); }
		if (error == GL_INVALID_FRAMEBUFFER_OPERATION) { printf("[OpenGL ERROR] Invalid Framebuffer Operation\n"); }
		assert(0);
	}

	int id = pixel[0] -1;
	return id;
	//	entity* e = get_entity(id);
	// printf(" -> id: %d, entity: %s, float: %f\n", id, e->name, pixel[0]);
}
#endif

// get -----------------------------------------------------------
int   get_camera_ent_id()		{ return camera_ent_idx; }
int** get_transparent_ids_ptr()	{ return &transparent_ents; }
int*  get_transparent_ids_len()	{ return &transparent_ents_len; }
int** get_dir_light_ids_ptr()	{ return &dir_lights; }
int*  get_dir_light_ids_len()	{ return &dir_lights_len; }
int** get_point_light_ids_ptr()	{ return &point_lights; }
int*  get_point_light_ids_len()	{ return &point_lights_len; }
int** get_spot_light_ids_ptr()	{ return &spot_lights; }
int*  get_spot_light_ids_len()	{ return &spot_lights_len; }

#ifdef EDITOR_ACT
bee_bool get_gamestate()
{
	return gamestate;
}

int* get_draw_calls_per_frame() { return &draw_calls_per_frame; }
int* get_verts_per_frame()		{ return &verts_per_frame; }
int* get_tris_per_frame()		{ return &tris_per_frame; }
#endif

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
f32* get_exposure()
{ return &exposure; }
u32 get_color_buffer()
{ return tex_col_buffer; }
void get_bg_color(vec3 col)
{
	glm_vec3_copy(bg_color, col);
}
// ---------------------------------------------------------------

// set -----------------------------------------------------------
void renderer_set(render_setting setting, bee_bool value)
{
	if (value == BEE_SWITCH)
	{
		renderer_set(setting, !*renderer_get(setting));
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
void set_camera_ent_id(int id)	{ camera_ent_idx = id; }

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
	gravity_reset_pending_actions();

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
void set_all_scripts(bee_bool act)
{
	int entity_ids_len = 0;
	int* entity_ids = get_entity_ids(&entity_ids_len);
	for (int i = 0; i < entity_ids_len; ++i)
	{
		entity* ent = get_entity(entity_ids[i]);
		for (int n = 0; n < ent->scripts_len; ++n)
		{
			ent->scripts[n]->active = act;
			if (!act)
			{
				// so the init gets run every time you press play
				ent->scripts[n]->init_closure_assigned   = BEE_FALSE;
				ent->scripts[n]->update_closure_assigned = BEE_FALSE;
				ent->scripts[n]->vm		 = NULL;
				ent->scripts[n]->closure = NULL;
			}
		}
	}
}
#endif

void set_bg_color(vec3 col)
{
	glm_vec3_copy(col, bg_color);
	glClearColor(bg_color[0], bg_color[1], bg_color[2], 1.0f);
}
// ---------------------------------------------------------------


// @UNCLEAR: still used ???
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