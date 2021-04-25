#include <stdio.h>

#include "glad/glad.h"

#include "shader.h"
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
u32 create_shader_from_file(const char* vert_path, const char* frag_path) // const char* vert_path, const char* frag_path
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

// activate / use the shader 
void shader_use(u32 shader)
{
	glUseProgram(shader);
}

// set a bool in the shader 
// the given int is used as the bool ( 0/1 )
void shader_set_bool(u32 shader, const char* name, int value)
{
	glUniform1i(glGetUniformLocation(shader, name), value);
}
// set an integer in the shader
void shader_set_int(u32 shader, const char* name, int value)
{
	glUniform1i(glGetUniformLocation(shader, name), value);
}
// set a float in the shader
void shader_set_float(u32 shader, const char* name, f32 value)
{
	glUniform1f(glGetUniformLocation(shader, name), value);
}
// set a vec2 in the shader
void shader_set_vec2_f(u32 shader, const char* name, f32 x, f32 y)
{
	glUniform2f(glGetUniformLocation(shader, name), x, y);
}
// set a vec2 in the shader
void shader_set_vec2(u32 shader, const char* name, vec2 v)
{
	glUniform2f(glGetUniformLocation(shader, name), v[0], v[1]);
}
// set a vec3 in the shader
void shader_set_vec3_f(u32 shader, const char* name, f32 x, f32 y, f32 z)
{
	glUniform3f(glGetUniformLocation(shader, name), x, y, z);
}
// set a vec3 in the shader
void shader_set_vec3(u32 shader, const char* name, vec3 v)
{
	glUniform3f(glGetUniformLocation(shader, name), v[0], v[1], v[2]);
}
// set a matrix 4x4 in the shader
void shader_set_mat4(u32 shader, const char* name, mat4 value)
{
	unsigned int transformLoc = glGetUniformLocation(shader, name);
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, value[0]);
}