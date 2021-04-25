#pragma once
#ifndef RENDERER_H
#define RENDERER_H

#include "CGLM/cglm.h"

#include "global.h"
#include "object_data.h"
#include "ui.h"


void renderer_init();

void renderer_update();

void renderer_cleanup();


void draw_mesh(mesh* _mesh, material* mat, vec3 pos, vec3 rot, vec3 scale, enum bee_bool rotate_global);


void add_entity(vec3 pos, vec3 rot, vec3 scale, mesh* _mesh, material* _material, light* _light, char* name);

void add_entity_cube();

entity_properties get_entity_properties(int index);

void get_renderer_properties(f32* _perspective, f32* _near_plane, f32* _far_plane);

void get_entity_len(int* _entities_len);


void renderer_enable_wireframe(enum bee_bool act);

#endif