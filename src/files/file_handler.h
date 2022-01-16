#pragma once
#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include "assimp/cimport.h"         // C importer interface
#include "assimp/scene.h"           // output data structure
#include "assimp/postprocess.h"     // post processing flags

#include "global.h"
#include "types/object_data.h"


// returns true if the file under the specified path "file_path" exists, otherwise false
bee_bool file_exists_check(const char* file_path);

// read a text-file and output all contents as a char*
// !!! free() the returned char* as it gets allocated
// taken from: https://github.com/jdah/minecraft-weekend/blob/master/src/gfx/shader.c
char* read_text_file(const char* file_path);
// read the given lengt of a text-file and output all contents as a char*
// !!! free() the returned char* as it gets allocated
// taken from: https://github.com/jdah/minecraft-weekend/blob/master/src/gfx/shader.c
char* read_text_file_len(const char* file_path, int* length);

// writes text "txt" into file at "file_path", creates file if it doesnt exist
// "len" is the length of "txt", or shorter if you want to cut off the string
void write_text_file(const char* file_path, const char* txt, int len);

// create copy of file at 'filepath_src' and copy it to 'filepath_cpy'
rtn_code copy_file(char* filepath_src, char* filepath_cpy);


// read and parse a .shader file
shader* shader_load_from_path(const char* file_path, const char* name);


// loads an image from file and loads it onto the gpu, 
// render by calling: glBindTexture(GL_TEXTURE_2D, u32);
// returns: u32 handle for the opengl texture
// hint --------------------------------------------------
// ! the image is expected to be in "(cwd)\assets\"
//u32 load_image(const char* image_name);

// !!! used internally !!!
// taken from jdh's minecraft clone: https://github.com/jdah/minecraft-weekend
void texture_load_pixels(char* path, u8** pixels_out, size_t* width_out, size_t* height_out, int* channel_num, bee_bool flip_vertical);
// !!! used internally !!!
// taken from jdh's minecraft clone: https://github.com/jdah/minecraft-weekend
u32 texture_create_from_pixels(u8* pixels, size_t width, size_t height, int channel_num, bee_bool gamma_correct);
// loads an image and creates a texture out of it
// taken from jdh's minecraft clone: https://github.com/jdah/minecraft-weekend
texture texture_create_from_path(const char* file_path, const char* name, bee_bool flip_vertical, bee_bool gamma_correct);


// loads 6 images and creates a cubemap texture out of it
u32 load_cubemap(char* right, char* left, char* bottom, char* top, char* front, char* back);


// load a mesh from the file under "file_path"
// taken from: https://learnopengl.com/Model-Loading/Model
mesh load_mesh(char* file_path);
// !!! used internally !!!
// taken from: https://learnopengl.com/Model-Loading/Model
void process_node(struct aiNode* node, const struct aiScene* scene, mesh* m);
// !!! used internally !!!
// taken from: https://learnopengl.com/Model-Loading/Model
mesh process_mesh(struct aiMesh* m_assimp, struct aiScene* scene);

#endif