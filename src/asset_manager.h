#pragma once
#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include "global.h"
#include "object_data.h"

void assetm_init();

// taken from: https://codeforwin.org/2018/03/c-program-to-list-all-files-in-a-directory-recursively.html
void search_dir(const char* dir_path);

void assetm_cleanup();

texture* get_all_textures(int* textures_len);

texture get_texture(const char* name);

void log_texture(const char* path, const char* name);
void create_texture(const char* name);

#endif
