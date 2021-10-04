#pragma once
#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include "global.h"
#include "object_data.h"

void test_serialization();

// serialization ------------------------------------------------

void serialize_scene(char* buffer, int* offset, scene* s);
void serialize_entity(char* buffer, int* offset, entity* ent);
void serialize_mesh(char* buffer, int* offset, mesh* m);
void serialize_material(char* buffer, int* offset, material* m);
void serialize_light(char* buffer, int* offset, light* l);
void serialize_camera(char* buffer, int* offset, camera* c);
void serialize_script(char* buffer, int* offset, gravity_script* s);
void serialize_texture(char* buffer, int* offset, texture* t);
void serialize_shader(char* buffer, int* offset, shader* s);

// ---- base types ----
void serialize_int(char* buffer, int* offset, int val);
void serialize_float(char* buffer, int* offset, f32 val);
void serialize_u32(char* buffer, int* offset, u32 val);
void serialize_enum(char* buffer, int* offset, char val);
void serialize_str(char* buffer, int* offset, char* val);
void serialize_vec2(char* buffer, int* offset, vec2 val);
void serialize_vec3(char* buffer, int* offset, vec3 val);

// deserialization ----------------------------------------------

scene	  deserialize_scene(char* buffer, int* offset, rtn_code* success);
entity	  deserialize_entity(char* buffer, int* offset);
material* deserialize_material(char* buffer, int* offset);
mesh*	  deserialize_mesh(char* buffer, int* offset);
camera	  deserialize_camera(char* buffer, int* offset);
light     deserialize_light(char* buffer, int* offset);

shader  deserialize_shader(char* buffer, int* offset);
texture deserialize_texture(char* buffer, int* offset);
gravity_script* deserialize_script(char* buffer, int* offset);

// ---- base types ----
int   deserialize_int(char* buffer, int* offset);
float deserialize_float(char* buffer, int* offset);
u32   deserialize_u32(char* buffer, int* offset);
char  deserialize_enum(char* buffer, int* offset);
char* deserialize_str(char* buffer, int* offset);
void  deserialize_vec2(char* buffer, int* offset, vec2 out);
void  deserialize_vec3(char* buffer, int* offset, vec3 out);
#endif