#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include "CGLM\cglm.h"

#include "global.h"

// ---- editor camera ----

// add to the current position
void editor_camera_move(vec3 dist);

void set_editor_camera_pos(vec3 pos);
void set_editor_camera_front(vec3 dir);
void set_editor_camera_up(vec3 dir);


// get the cameras current position
void get_editor_camera_pos(vec3* pos);

// get the cameras current front direction
void get_editor_camera_front(vec3* dir);

// get the cameras current up direction
void get_editor_camera_up(vec3* dir);

// get the direction from the taget towards the camera (normalized)
void get_editor_camera_inv_direction(vec3* dir);

// get the cameras right axis (normalized)
void get_editor_camera_right_axis(vec3* axis);

// get the cameras up axis (normalized)
void get_editor_camera_up_axis(vec3* axis);

void get_editor_camera_turntable_view_mat(const float radius, mat4* dest);

// normal view matrix
void get_editor_camera_view_mat(mat4* view);

// ---- camera component ----
typedef struct entity entity;
typedef float vec3[3];
// get the direction from the taget towards the camera (normalized)
void get_camera_inv_direction(entity* cam, vec3* dir);

// get the cameras right axis (normalized)
void get_camera_right_axis(entity* cam, vec3* axis);

// get the cameras up axis (normalized)
void get_camera_up_axis(entity* cam, vec3* axis);

// normal view matrix
void get_camera_view_mat(entity* cam, mat4* view);

#endif