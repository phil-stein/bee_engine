#include "app.h"

#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include <string.h>

#include "GLAD/glad.h"
#include "tinyfd/tinyfiledialogs.h"

#include "input.h"
#include "shader.h"
#include "window.h"
#include "camera.h"
#include "renderer.h"
#include "entities.h"
#include "game_time.h"
#include "editor_ui.h"
#include "object_data.h"
#include "file_handler.h"
#include "asset_manager.h"
#include "scene_manager.h"
#include "gravity_script.h"


#pragma region __VARS__

// ---- vars ----
GLFWwindow* window;

bee_bool fly_enabled = BEE_FALSE;

// settings
f32 free_look_mouse_sensitivity = 0.125;
int mouse_callback_idx = 0;

#pragma endregion


void init()
{
	window = get_window();

	// ---- populate scene ----
	/*
	shader* shader_default = add_shader("basic.vert", "blinn_phong.frag", "SHADER_default");

	shader* shader_unlit   = add_shader("basic.vert", "unlit.frag", "SHADER_unlit");
						  
	shader* shader_cel     = add_shader("basic.vert", "cel_shading.frag", "SHADER_cel");
						  
	shader* shader_noise   = add_shader("basic.vert", "basic_noise.frag", "SHADER_noise");
	

	vec2 tile = { 1.0f, 1.0f };
	vec3 tint = { 1.0f, 1.0f, 1.0f };
	
	texture blank_tex = get_texture("blank.png");
	material* mat_blank		  = add_material(shader_default, blank_tex, blank_tex, BEE_FALSE, 1.0f, tile, tint, BEE_FALSE, "MAT_blank", BEE_TRUE);
	material* mat_blank_unlit = add_material(shader_unlit, blank_tex, blank_tex, BEE_FALSE, 1.0f, tile, tint, BEE_FALSE, "MAT_blank_unlit", BEE_TRUE);
	material* mat_cel		  = add_material(shader_cel, blank_tex, blank_tex, BEE_FALSE, 1.0f, tile, tint, BEE_FALSE, "MAT_cel", BEE_TRUE);
	*/
	/*
	material* mat_noise		  = add_material(shader_noise, blank_tex, blank_tex, BEE_FALSE, 1.0f, tile, tint, BEE_FALSE, "MAT_noise", BEE_TRUE);
	
	// load texture
	texture crate_dif_tex   = get_texture("crate01_dif.png");
	texture crate_spec_tex  = get_texture("crate01_spec.png");
	material* mat_crate		= add_material(get_shader("SHADER_default"), crate_dif_tex, crate_spec_tex, BEE_FALSE, 1.0f, tile, tint, BEE_FALSE, "MAT_crate", BEE_TRUE);

	texture grass_dif_tex  = get_texture("grass01_dif.png");
	texture grass_spec_tex = get_texture("grass01_spec.png");
	material* mat_grass	   = add_material(shader_default, grass_dif_tex, grass_spec_tex, BEE_FALSE, 1.0f, tile, tint, BEE_FALSE, "MAT_grass", BEE_TRUE);
	
	texture barrel_dif_tex  = get_texture("barrel01_dif.png");
	texture barrel_spec_tex = get_texture("barrel01_spec.png");
	material* mat_barrel	= add_material(shader_default, barrel_dif_tex, barrel_spec_tex, BEE_FALSE, 1.0f, tile, tint, BEE_FALSE, "MAT_barrel", BEE_TRUE);
	
	texture robot_dif_tex  = get_texture("robot01_dif.png");
	texture robot_spec_tex = get_texture("robot01_spec.png");
	material* mat_robot	   = add_material(shader_default, robot_dif_tex, robot_spec_tex, BEE_FALSE, 1.0f, tile, tint, BEE_FALSE, "MAT_robot", BEE_TRUE);

	add_material(shader_default, get_texture("demon_diffuse.png"), get_texture("blank_black.png"), BEE_FALSE, 1.0f, tile, tint, BEE_FALSE, "MAT_demon", BEE_TRUE);

	// mesh m_cube = make_cube_mesh();


	texture glass_dif_tex = get_texture("window.png");
	material* mat_glass	  = add_material(shader_default, glass_dif_tex, blank_tex, BEE_TRUE, 1.0f, tile, tint, BEE_TRUE, "MAT_glass", BEE_TRUE);

	//
	// @TODO: make the renderer use the right camera
	// 
	// in-game-camera
	camera cam = make_camera(40.0f, 0.1f, 100.0f);
	cam.front[0] = 0; 
	cam.front[1] = -.25f; 
	cam.front[2] = -1; 
	vec3 cam_pos   = { 0.0f, 2.0f,  3.5f };
	vec3 cam_rot   = { 0, 0, 0 };
	vec3 cam_scale = { 1, 1, 1 };
	mesh* m_camera = get_mesh("camera.obj");
	// add_entity(cam_pos, cam_rot, cam_scale, m_camera, get_material("MAT_cel"), &cam, NULL, "camera");
	add_entity(cam_pos, cam_rot, cam_scale, NULL, NULL, &cam, NULL, "camera");

	int ent_empty = add_entity(NULL, NULL, NULL, NULL, NULL, NULL, NULL, "game controller");
	entity_add_script(ent_empty, "game_controller_tmp.gravity");


	vec3 ambient      = { 0.0f, 0.0f, 0.0f };
	vec3 diffuse01    = { 1.0f, 1.0f, 1.0f };
	vec3 diffuse02    = { 0.9f, 0.25f, 0.85f }; //rgb(231, 65, 218)
	vec3 specular     = { 1.0f, 1.0f, 1.0f };
	vec3 direction	  = { -0.2f, -1.0f, -0.3f };
	vec3 direction02  = { -0.2f, -0.8f, -0.5f };
	light dir_light   = make_dir_light(ambient, diffuse01, specular, direction);
	light point_light = make_point_light(ambient, diffuse02, specular, 1.0f, 0.14f, 0.13f); // 0.09f, 0.032f);
	light spot_light  = make_spot_light(ambient, diffuse01, specular, direction, 1.0f, 0.09f, 0.032f, 0.91f, 0.82f);

	vec3 pos_light01   = { 0.0f, 2.0f, 0.0f };
	vec3 pos_light02   = { 0.75f, 0.0f, 0.0f };
	vec3 pos_light03   = { -1.0f, 0.75f, 0.0f };
	vec3 rot_light	   = { 0.0f, 0.0f,  0.0f };
	vec3 scale		   = { 1.0f, 1.0f, 1.0f };
	glm_vec3_copy(diffuse01, mat_blank->tint);
	// add_entity(pos_light01, rot_light, scale_light02, m_arrow,      get_material("MAT_cel"), NULL, &dir_light,   "dir_light");	// mat_blank_unlit
	add_entity(pos_light01, rot_light, scale, NULL, NULL, NULL, &dir_light,   "dir_light");	// mat_blank_unlit
	// add_entity(pos_light03, rot_light, scale_light02, m_flashlight, get_material("MAT_cel"), NULL, &spot_light,  "spot_light");	// mat_blank_unlit
	add_entity(pos_light03, rot_light, scale, NULL, NULL, NULL, &spot_light,  "spot_light");	// mat_blank_unlit
	glm_vec3_copy(diffuse02, mat_blank->tint);
	// add_entity(pos_light02, rot_light, scale_light01, m_lightbulb, get_material("MAT_cel"), NULL, &point_light, "point_light");	// mat_blank_unlit
	add_entity(pos_light02, rot_light, scale, NULL, NULL, NULL, &point_light, "point_light");	// mat_blank_unlit
	glm_vec3_copy(specular, mat_blank->tint); // all 1.0f

	// plane
	mesh m_plane = make_plane_mesh();
	vec3 pos01 = { 0.0f, -0.5f, 0.0f };
	vec3 pos02 = { -1.5f, 0.0f, 1.5f };
	vec3 pos03 = { -1.5f, 0.25f, 1.5f };
	vec3 pos04 = { -1.5f, 0.5f, 1.5f };
	vec3 rot01 = { 0.0f, 0.0f, 0.0f };
	vec3 scale01;
	glm_vec3_scale(scale, 5.0f, scale01);
	add_entity(pos01, rot01, scale01, &m_plane, get_material("MAT_grass"), NULL, NULL, "ground");  // mat_grass
	int ent_win01 = add_entity(pos02, rot01, scale, &m_plane, get_material("MAT_glass"), NULL, NULL, "window_01"); // mat_glass
	int ent_win02 = add_entity(pos03, rot01, scale, &m_plane, get_material("MAT_glass"), NULL, NULL, "window_02"); // mat_glass
	int ent_win03 = add_entity(pos04, rot01, scale, &m_plane, get_material("MAT_glass"), NULL, NULL, "window_03"); // mat_glass
	entity_set_parent(ent_win03, ent_win02);
	entity_set_parent(ent_win02, ent_win01);

	mesh* m_crate = get_mesh("crate01.obj");
	vec3 pos05 = { 1.0f, 0.0f, 1.0f };
	vec3 scale02;
	glm_vec3_scale(scale, 0.5f, scale02);
	int ent_crate = add_entity(pos05, rot01, scale02, m_crate, get_material("MAT_crate"), NULL, NULL, "crate"); // mat_crate
	entity* crate_ptr = get_entity(ent_crate);
	crate_ptr->_mesh.visible = BEE_FALSE;
	
	mesh* m_robot = get_mesh("robot01_LD.obj");
	vec3 pos08 = { 2.0f, -0.5f, 0.0f };
	vec3 scale04;
	glm_vec3_scale(scale, 0.25f, scale04);
	int ent_robot = add_entity(pos08, rot01, scale04, m_robot, get_material("MAT_robot"), NULL, NULL, "robot"); // mat_robot
	// entity_add_script(ent_robot, "move_arrows.gravity");


	mesh* m_demon = get_mesh("Eye-Char01.obj");
	vec3 pos09 = { 0.0f, -1.2f, 0.0f };
	vec3 rot02 = { 0.0f, 180,   0.0f };
	// vec3 scale04;
	// glm_vec3_scale(scale, 0.25f, scale04);
	int ent_demon = add_entity(pos09, rot02, scale04, m_demon, get_material("MAT_demon"), NULL, NULL, "demon"); // mat_robot
	entity_add_script(ent_demon, "move_arrows.gravity");


	mesh* m_bunny = get_mesh("bunny.obj");
	vec3 pos06 = { 1.5f, -0.5f, -1.5f };
	int ent_bunny = add_entity(pos06, rot01, scale, m_bunny, get_material("MAT_blank"), NULL, NULL, "bunny"); // mat_blank
	entity_add_script(ent_bunny, "sinewave_y.gravity");

	mesh* m_barrel = get_mesh("post_apocalyptic_barrel.obj");
	vec3 pos07 = { -1.5f, 0.0f, -1.5f };
	vec3 scale03;
	glm_vec3_scale(scale, 0.5f, scale03);
	int ent_barrel = add_entity(pos07, rot01, scale03, m_barrel, get_material("MAT_barrel"), NULL, NULL, "barrel"); // mat_barrel
	entity_add_script(ent_barrel, "sinewave_x.gravity");
		
	entity_set_parent(ent_crate, ent_bunny);
	// entity_set_parent(ent_bunny, ent_crate);
	// entity_set_parent(ent_robot, ent_crate);
	
	// cube for testing noise shader
	add_entity(scale, rot01, scale, get_mesh("cube.obj"), get_material("MAT_noise"), NULL, NULL, "noise_cube"); // mat_barrel

	*/

	// load_scene("showcase06.scene");
	load_scene("physics05.scene");

	// generate physics components old
	/*
	entity* cube	  = get_entity(0);
	entity* ground	  = get_entity(2);
	entity* tombstone = get_entity(6);
	
	// rigidbody
	rigidbody rb01;
	vec3 f1 = { 100.0f, 500.0f, 0.0f };
	glm_vec3_copy(f1, rb01.force);
	glm_vec3_copy(GLM_VEC3_ZERO, rb01.velocity); 
	rb01.mass = 1.0f;
	cube->has_rb = BEE_TRUE;
	cube->rb = rb01;

	rigidbody rb02;
	vec3 f2 = { -100.0f * 5.0f, 500.0f * 5.0f, 0.0f };
	glm_vec3_copy(f2, rb02.force);
	glm_vec3_copy(GLM_VEC3_ZERO, rb02.velocity);
	rb02.mass = 5.0f;
	// tombstone->has_rb = BEE_TRUE;
	// tombstone->rb = rb02;

	// collider
	sphere_collider d_col_s;
	d_col_s.radius = 1.0f;
	box_collider c_col, d_col, g_col;
	glm_vec3_copy(GLM_VEC3_ZERO, c_col.aabb[0]);
	glm_vec3_copy(GLM_VEC3_ONE,  c_col.aabb[1]);
	glm_vec3_copy(GLM_VEC3_ZERO, d_col.aabb[0]);
	glm_vec3_copy(GLM_VEC3_ONE,  d_col.aabb[1]);
	vec3 min = { -5, 0, -5 };
	vec3 max = {  5, 1,  5 };
	glm_vec3_copy(min, g_col.aabb[0]);
	glm_vec3_copy(max, g_col.aabb[1]);
	
	collider c01, c02, c03;
	c01.type = BOX_COLLIDER;
	// c01.sphere = c_col;
	c01.box = c_col;
	glm_vec3_copy(GLM_VEC3_ZERO, c01.offset);
	c02.type =  BOX_COLLIDER; // SPHERE_COLLIDER;
	// c02.sphere = d_col_s;
	c02.box = d_col;
	vec3 offset2 = { 0, 1, 0 };
	glm_vec3_copy(offset2, c02.offset);

	c03.type = BOX_COLLIDER;
	c03.box = g_col;
	vec3 offset3 = { 0, 0, 0 };
	glm_vec3_copy(offset3, c03.offset);
	
	cube->has_collider = BEE_TRUE;
	cube->collider = c01;
	tombstone->has_collider = BEE_TRUE;
	tombstone->collider = c02;
	ground->has_collider = BEE_TRUE;
	ground->collider = c03;
	*/


#ifdef EDITOR_ACT
	// glfwSetCursorPosCallback(window, rotate_cam_by_mouse);
	mouse_callback_idx = register_mouse_pos_callback(rotate_editor_cam_by_mouse);
#endif

}

void update()
{
#ifdef EDITOR_ACT
	// ---- input ----
	process_input(window);
#endif
	
	// ---- fps ----
	
	char title[50];
	int rtn = sprintf_s(title, sizeof(title), "bee engine | fps: %d | scene: \"%s\"", (int)get_fps(), get_active_scene_name());
	assert(rtn != 0);

	set_window_title(title);

	// -------------

}

void cleanup()
{
	// de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------

}


// ---- funcs ----

settings get_settings()
{
	settings _settings;

	_settings.free_look_mouse_sensitivity = &free_look_mouse_sensitivity;

	return _settings;
}

// handle input
#ifdef EDITOR_ACT
void process_input(GLFWwindow* window)
{

	#pragma region CAM_WASD_&_QE_INPUT
	// move the cam
	float cam_speed = 2.5f * get_delta_time();
	if (is_key_down(KEY_LeftShift))
	{
		cam_speed *= 3;
	}
	if (is_key_down(KEY_W))
	{
		vec3 front; get_editor_camera_front(&front);
		glm_vec3_scale(front, cam_speed, front);

		editor_camera_move(front);
	}
	if (is_key_down(KEY_S))
	{
		vec3 front; get_editor_camera_front(&front);
		glm_vec3_scale(front, -cam_speed, front);

		editor_camera_move(front);
	}
	if (is_key_down(KEY_A))
	{
		vec3 up;    get_editor_camera_up(&up);
		vec3 front; get_editor_camera_front(&front);

		vec3 dist;
		glm_vec3_cross(front, up, dist);
		glm_vec3_normalize(dist);
		glm_vec3_scale(dist, -cam_speed, dist);

		editor_camera_move(dist);
	}
	if (is_key_down(KEY_D))
	{
		vec3 up;    get_editor_camera_up(&up);
		vec3 front; get_editor_camera_front(&front);

		vec3 dist;
		glm_vec3_cross(front, up, dist);
		glm_vec3_normalize(dist);
		glm_vec3_scale(dist, cam_speed, dist);

		editor_camera_move(dist);
	}
	if (is_key_down(KEY_Q))
	{
		vec3 up;	get_editor_camera_up(&up);
		glm_vec3_scale(up, -cam_speed, up);

		editor_camera_move(up);
	}
	if (is_key_down(KEY_E))
	{
		vec3 up; get_editor_camera_up(&up);
		glm_vec3_scale(up, cam_speed, up);

		editor_camera_move(up);
	}
	#pragma endregion

	if (is_key_pressed(KEY_F10))
	{
		// tinyfd_beep(); // doesnt work
		// tinyfd_notifyPopup("window title", "hi there \nsecond line", "info"); // "info" "warning" "error"
		// tinyfd_messageBox("window title", "hi there \nsecond line", "okcancel", "info", 1);
		// char* input = tinyfd_inputBox("window title", "hi there", "text here ...");
		// printf("input from input_box: %s", input);
		
		// char* path = tinyfd_saveFileDialog(
		// 	"window title", "C:\\Workspace\\C\\BeeEngine\\assets", 0, // 0 (1 in the following example)
		// 	NULL, // NULL or char const * lFilterPatterns[1]={"*.txt"};
		// 	NULL); // NULL or "text files"
		// printf("path from save_file_window: \"%s\"", path);

		// char* path = tinyfd_openFileDialog(
		// 	"window title", "C:\\Workspace\\C\\BeeEngine\\assets", 0, // 0 (2 in the following example)
		// 	NULL, // NULL or char const * lFilterPatterns[2]={"*.png","*.jpg"};
		// 	NULL, // NULL or "image files"
		// 	BEE_FALSE); // 0
		// printf("path from open_file_window: \"%s\"", path);
		
		// material* mat = get_material("MAT_rename");
		// printf("material 'MAT_rename' name: %s\n", mat->name);

		submit_txt_console("pressed F10");
	}

	// wireframe- / solid-mode switch on tab
	if (is_key_pressed(KEY_Tab))
	{
		renderer_set(RENDER_WIREFRAME, BEE_SWITCH);
	}

	// maximized- / minimized window switch on f11
	if (is_key_pressed(KEY_F11))
	{
		set_window_maximized(BEE_SWITCH);
	}

	// fly-mode, rotate cam with mouse
	if (is_mouse_pressed(MOUSE_right))
	{
		fly_enabled = BEE_TRUE;
		set_cursor_visible(BEE_FALSE);
	}
	else if (is_mouse_released(MOUSE_right) && fly_enabled)
	{
		center_cursor_pos();
		fly_enabled = BEE_FALSE;
		set_cursor_visible(BEE_TRUE);
	}
}
#endif

#ifdef EDITOR_ACT
// rotates the camera accoding to the mouse-movement
void rotate_editor_cam_by_mouse()
{
	if (fly_enabled == BEE_FALSE)
	{
		return;
	}



	static bee_bool init = BEE_FALSE;
	static f32 pitch, yaw;

	f32 xoffset = get_mouse_delta_x();
	f32 yoffset = get_mouse_delta_y();

	xoffset *= free_look_mouse_sensitivity;
	yoffset *= free_look_mouse_sensitivity;

	
	yaw += xoffset;
	pitch += yoffset;

	// printf("pitch: %f, yaw: %f\n", pitch, yaw);

	if (pitch > 89.0f)
	{ pitch = 89.0f; }
	if (pitch < -89.0f)
	{ pitch = -89.0f; }

	if (!init)
	{
		// @TODO: get the cameras current pitch, yaw
		pitch = -30.375f;
		yaw	  =	-90.875;
		init = BEE_TRUE;
	}

	vec3 dir;
	f32 yaw_rad = yaw;     glm_make_rad(&yaw_rad);
	f32 pitch_rad = pitch; glm_make_rad(&pitch_rad);

	dir[0] = (f32)cos(yaw_rad) * (f32)cos(pitch_rad);
	dir[1] = (f32)sin(pitch_rad);
	dir[2] = (f32)sin(yaw_rad) * (f32)cos(pitch_rad);
	set_editor_camera_front(dir);
}
#endif

// @TODO: move this to a util file
// returns: random number with range 0.0 - 1.0
f32 rnd_num()
{
	// limits the random int returned by rand to a 0.0 - 1.0 range
	return (f32)rand() / RAND_MAX;
}