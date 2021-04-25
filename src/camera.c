#include "camera.h"

#define GLFW_INCLUDE_NONE
#include "GLFW\glfw3.h"

#include "global.h"


// ---- vars ----
vec3 position = { 0.15f, 3.75f,  5.80f };
vec3 front	  = { 0.0f,  -0.5f, -0.85f };
vec3 up		  = { 0.0f,  1.0f,   0.0f  };

vec3 target   = { 0.0, 0.0, 0.0 };


// ---- func ----
void camera_move(vec3 dist)
{
	glm_vec3_add(position, dist, position);
}

// ---- set ----
void set_camera_pos(vec3 pos)
{
	glm_vec3_copy(pos, position);
}
void set_camera_front(vec3 dir)
{
	glm_vec3_copy(dir, front);
}
void set_camera_up(vec3 dir)
{
	glm_vec3_copy(dir, up);
}

// ---- get ----
void get_camera_pos(vec3* pos)
{
	glm_vec3_copy(position, pos);
}
void get_camera_front(vec3* dir)
{
	glm_vec3_copy(front, dir);
}
void get_camera_up(vec3* dir)
{
	glm_vec3_copy(up, dir);
}

void get_camera_inv_direction(vec3* dir)
{
	// same as: dir = position - target; 
	glm_vec3_sub(position, target, dir);
	glm_vec3_normalize(dir);
}

void get_camera_right_axis(vec3* axis)
{
	vec3 up = { 0.0, 1.0, 0.0 };
	vec3 dir = { 0.0, 0.0, 0.0 };
	get_camera_inv_direction(dir);

	// axis = { 0.0, 0.0, 0.0 };
	glm_vec3_cross(up, dir, axis);
	glm_vec3_normalize(axis);
}

void get_camera_up_axis(vec3* axis)
{
	vec3 dir = { 0.0, 0.0, 0.0 }; 
	get_camera_inv_direction(dir);

	vec3 right = { 0.0, 0.0, 0.0 }; 
	get_camera_right_axis(right);

	glm_vec3_cross(dir, right, axis);
}

void get_camera_turntable_view_mat(const float radius, mat4* view)
{
	// example of usage
	// turntable view matrix ----------
	// vec4* vec_mat = get_camera_turntable_view_mat(5.0);
	// mat4 view;
	// glm_mat4_copy(vec_mat, view);
	
	f32 cam_x = (f32)sin(glfwGetTime()) * radius;
	f32 cam_z = (f32)cos(glfwGetTime()) * radius;

	vec3 pos = { cam_x, 0.0, cam_z };
	// get_camera_pos(pos);
	// pos[1] = 0.0; // y component
	vec3 center = { 0.0, 0.0, 0.0 };
	vec3 up = { 0.0, 1.0, 0.0 };
	glm_lookat(pos, center, up, view);
}

void get_camera_view_mat(mat4* view)
{
	vec3 center;
	glm_vec3_add(position, front, center);
	glm_lookat(position, center, up, view);
}
