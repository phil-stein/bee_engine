#include <stdio.h>

#include "glad/glad.h"

#include "shader.h"
#include "str_util.h"
#include "stb/stb_ds.h"
#include "asset_manager.h"
// #include "file_handler.h"

// generate a shader-program from a vertex- and fragment-shader
// returns: a pointer to the opengl shader program as a "unsigned int" aka. "u32"
u32 create_shader(char* vert_shader_src, char* frag_shader_src)
{
	// build and compile our shader program
	// ------------------------------------

	// vertex shader
	u32 vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vert_shader_src, NULL);
	glCompileShader(vertexShader);

	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		fprintf(stderr, "-!!!-> ERROR_VERTEX_COMPILATION: \n -> %s\n", infoLog);
	}

	// fragment shader
	u32 fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &frag_shader_src, NULL);
	glCompileShader(fragmentShader);

	// check for shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		fprintf(stderr, "%s\n-!!!-> ERROR_FRAGMENT_COMPILATION: \n -> %s\n", vert_shader_src, infoLog);
	}

	// link shaders
	u32 shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		fprintf(stderr, "-!!!-> ERROR_PROGRAM_LINKING: \n -> %s\n", infoLog);
	}

	// free the shaders
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

// generate a shader-program from a vertex- and fragment-shader
// returns: a pointer to the opengl shader program as a "unsigned int" aka. "u32"
u32 create_shader_from_file_handle(const char* vert_path, const char* frag_path) // const char* vert_path, const char* frag_path
{
	// read the shader from file
	// ------------------------------------
	// const char* vert_shader_src_read = read_text_file(vert_path);
	// const char* frag_shader_src_read = read_text_file(frag_path);
	
	// ---- vert ---- 
	 
	FILE* f;
	char* vert_src;
	long len;

	f = fopen(vert_path, "rb");
	if (f == NULL) {
		fprintf(stderr, "error loading vert shader text-file at: %s\n", vert_path);
		assert(BEE_FALSE);
	}

	// get len of file
	fseek(f, 0, SEEK_END);
	len = ftell(f);
	assert(len > 0);
	fseek(f, 0, SEEK_SET);

	// alloc memory 
	vert_src = calloc(1, len);
	assert(vert_src != NULL);

	// fill text buffer
	fread(vert_src, 1, len, f);
	assert(strlen(vert_src) > 0);
	fclose(f);

	// --------------

	// ---- frag ----

	char* frag_src;
	len = 0;

	f = fopen(frag_path, "rb");
	if (f == NULL) {
		fprintf(stderr, "error loading frag shader text-file at: %s\n", frag_path);
		assert(BEE_FALSE);
	}

	// get len of file
	fseek(f, 0, SEEK_END);
	len = ftell(f);
	assert(len > 0);
	fseek(f, 0, SEEK_SET);

	// alloc memory 
	frag_src = calloc(1, len);
	assert(frag_src != NULL);

	// fill text buffer
	fread(frag_src, 1, len, f);
	assert(strlen(frag_src) > 0);
	fclose(f);

	// --------------

	// print the read shader source code for debugging
	// printf("\n---- vert shader ---- \n%s\n", vert_shader_src);
	// printf("\n---- frag shader ---- \n%s\n\n", frag_shader_src);

	u32 shader = create_shader(vert_src, frag_src); // vert_shader_src, frag_shader_src_shaded

	// remember to free the memory allocated by read_text_file()
	free(vert_src); 
	free(frag_src); 

	return shader;
}

// generate a shader-program from a vertex- and fragment-shader
// returns: a pointer to the opengl shader program as a "unsigned int" aka. "u32"
shader create_shader_from_file(const char* vert_path, const char* frag_path, const char* name) // const char* vert_path, const char* frag_path
{
	// read the shader from file
	// ------------------------------------
	// const char* vert_shader_src_read = read_text_file(vert_path);
	// const char* frag_shader_src_read = read_text_file(frag_path);

	// ---- vert ---- 

	FILE* f;
	char* vert_src;
	long len;

	f = fopen(vert_path, "rb");
	if (f == NULL) {
		fprintf(stderr, "error loading vert shader text-file at: %s\n", vert_path);
		assert(BEE_FALSE);
	}

	// get len of file
	fseek(f, 0, SEEK_END);
	len = ftell(f);
	assert(len > 0);
	fseek(f, 0, SEEK_SET);

	// alloc memory 
	vert_src = calloc(1, len);
	assert(vert_src != NULL);

	// fill text buffer
	fread(vert_src, 1, len, f);
	assert(strlen(vert_src) > 0);
	fclose(f);

	// --------------

	// ---- frag ----

	char* frag_src;
	len = 0;

	f = fopen(frag_path, "rb");
	if (f == NULL) {
		fprintf(stderr, "error loading frag shader text-file at: %s\n", frag_path);
		assert(BEE_FALSE);
	}

	// get len of file
	fseek(f, 0, SEEK_END);
	len = ftell(f);
	assert(len > 0);
	fseek(f, 0, SEEK_SET);

	// alloc memory 
	frag_src = calloc(1, len);
	assert(frag_src != NULL);

	// fill text buffer
	fread(frag_src, 1, len, f);
	assert(strlen(frag_src) > 0);
	fclose(f);

	// --------------

	u32 handle = create_shader(vert_src, frag_src);

	shader s;
	s.handle = handle;
	s.name = name;
	s.use_lighting = BEE_TRUE;
	s.uniform_defs = NULL;
	s.uniform_defs_len = 0;
	// char* frag_name = str_find_last_of(frag_path, "blinn_phong_top.frag");
	// if (frag_name != NULL)
	// {
	// 	printf(" -> blinn phong top\n");
	// 	uniform dif;
	// 	dif.name = "material02.diffuse";
	// 	dif.type = UNIFORM_TEX;
	// 	dif.tex_val = get_texture("cliff01_dif.png");
	// 	arrput(s.uniforms, dif);
	// 	s.uniforms_len++;
	// 	uniform spec;
	// 	spec.name = "material02.specular";
	// 	spec.type = UNIFORM_TEX;
	// 	spec.tex_val = get_texture("cliff01_spec.png");
	// 	arrput(s.uniforms, spec);
	// 	s.uniforms_len++;
	// 	uniform shininess;
	// 	shininess.name = "material02.shininess";
	// 	shininess.type = UNIFORM_F32;
	// 	shininess.f32_val = 1.0f;
	// 	arrput(s.uniforms, shininess);
	// 	s.uniforms_len++;
	// 	uniform tile;
	// 	tile.name = "material02.tile";
	// 	tile.type = UNIFORM_VEC2;
	// 	vec2 val2 = { 1, 1 };
	// 	glm_vec2_copy(val2, tile.vec2_val);
	// 	arrput(s.uniforms, tile);
	// 	s.uniforms_len++;
	// 	uniform tint;
	// 	tint.name = "material02.tint";
	// 	tint.type = UNIFORM_VEC3;
	// 	vec3 val3 = { 1, 1, 1 };
	// 	glm_vec3_copy(val3, tint.vec3_val);
	// 	arrput(s.uniforms, tint);
	// 	s.uniforms_len++;
	// 
	// }

	// remember to free the memory allocated by read_text_file()
	free(vert_src);
	free(frag_src);

	return s;
}

// activate / use the shader 
void shader_use(shader* s)
{
	glUseProgram(s->handle);
}

// set a bool in the shader 
// the given int is used as the bool ( 0/1 )
void shader_set_bool(shader* s, const char* name, int value)
{
	glUniform1i(glGetUniformLocation(s->handle, name), value);
}
// set an integer in the shader
void shader_set_int(shader* s, const char* name, int value)
{
	glUniform1i(glGetUniformLocation(s->handle, name), value);
}
// set a float in the shader
void shader_set_float(shader* s, const char* name, f32 value)
{
	glUniform1f(glGetUniformLocation(s->handle, name), value);
}
// set a vec2 in the shader
void shader_set_vec2_f(shader* s, const char* name, f32 x, f32 y)
{
	glUniform2f(glGetUniformLocation(s->handle, name), x, y);
}
// set a vec2 in the shader
void shader_set_vec2(shader* s, const char* name, vec2 v)
{
	glUniform2f(glGetUniformLocation(s->handle, name), v[0], v[1]);
}
// set a vec3 in the shader
void shader_set_vec3_f(shader* s, const char* name, f32 x, f32 y, f32 z)
{
	glUniform3f(glGetUniformLocation(s->handle, name), x, y, z);
}
// set a vec3 in the shader
void shader_set_vec3(shader* s, const char* name, vec3 v)
{
	glUniform3f(glGetUniformLocation(s->handle, name), v[0], v[1], v[2]);
}
// set a matrix 4x4 in the shader
void shader_set_mat4(shader* s, const char* name, mat4 value)
{
	unsigned int transformLoc = glGetUniformLocation(s->handle, name);
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, value[0]);
}