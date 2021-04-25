#pragma once
#ifndef SHADER_H
#define SHADER_H

#include"CGLM/cglm.h"
#include "global.h"

// !!! currently not reading from file
// generate a shader-program from a vertex- and fragment-shader
// give the filepath to the vert / frag shader as the arguments
// returns: a pointer to the opengl shader program as a "unsigned int" aka. "u32"
u32 create_shader_from_file(const char* vert_path, const char* frag_path);


// generate a shader-program from a vertex- and fragment-shader
// give the actual source code of the shader as arguments
// returns: a pointer to the opengl shader program as a "unsigned int" aka. "u32"
u32 create_shader(char* vert_shader_src, char* frag_shader_src);


// activate / use the shader 
void shader_use(u32 shader);

// set a bool in the shader 
// the given int is used as the bool ( 0/1 )
void shader_set_bool(u32 shader, const char* name, int value);
// set an integer in the shader
void shader_set_int(u32 shader, const char* name, int value);
// set a float in the shader
void shader_set_float(u32 shader, const char* name, f32 value);
// set a vec2 in the shader
void shader_set_vec2_f(u32 shader, const char* name, f32 x, f32 y);
// set a vec2 in the shader
void shader_set_vec2(u32 shader, const char* name, vec2 v);
// set a vec3 in the shader
void shader_set_vec3_f(u32 shader, const char* name, f32 x, f32 y, f32 z);
// set a vec3 in the shader
void shader_set_vec3(u32 shader, const char* name, vec3 v);
// set a matrix 4x4 in the shader
void shader_set_mat4(u32 shader, const char* name, mat4 value);

#endif