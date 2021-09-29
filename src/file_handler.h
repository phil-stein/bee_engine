#pragma once
#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include "assimp/cimport.h"         // C importer interface
#include "assimp/scene.h"           // output data structure
#include "assimp/postprocess.h"     // post processing flags

#include "global.h"
#include "object_data.h"


bee_bool file_exists_check(const char* file_path);

// read a text-file and output all contents as a char*
// taken from: https://github.com/jdah/minecraft-weekend/blob/master/src/gfx/shader.c
// warning --------------------------------------------
// !!! free() the returned char* as it gets allocated
char* read_text_file(const char* file_path);

// create copy of file at 'filepath_src' and copy it to 'filepath_cpy'
rtn_code copy_file(char* filepath_src, char* filepath_cpy);

// loads an image from file and loads it onto the gpu, 
// render by calling: glBindTexture(GL_TEXTURE_2D, u32);
// returns: u32 handle for the opengl texture
// hint --------------------------------------------------
// ! the image is expected to be in "(cwd)\assets\"
//u32 load_image(const char* image_name);

// taken from jdh's minecraft clone: https://github.com/jdah/minecraft-weekend
void texture_load_pixels(char* path, u8** pixels_out, size_t* width_out, size_t* height_out, int* channel_num, bee_bool flip_vertical);
u32 texture_create_from_pixels(u8* pixels, size_t width, size_t height, int channel_num);
texture texture_create_from_path(const char* file_path, const char* name, bee_bool flip_vertical);

// u32 load_cubemap();
u32 load_cubemap(char* right, char* left, char* bottom, char* top, char* front, char* back);


// load a mesh from an ".obj" file
// taken from: https://learnopengl.com/Model-Loading/Model
mesh load_mesh(char* file_path);
void process_node(struct aiNode* node, const struct aiScene* scene, mesh* m);
mesh process_mesh(struct aiMesh* m_assimp, struct aiScene* scene);

#endif