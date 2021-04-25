#pragma once
#ifndef APP_H
#define APP_H

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "CGLM/cglm.h"

#include "global.h"
#include "object_data.h"
#include "ui.h"


// struct used to pass pointers to various variables
typedef struct 
{

	f32* free_look_mouse_sensitivity;

}settings;


void init();

void update();

void cleanup();


settings get_settings();


void process_input(GLFWwindow* window);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void reset_cursor_pos();

f32 rnd_num();

#endif