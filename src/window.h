#pragma once
#ifndef WINDOW_H
#define WINDOW_H

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include "global.h"

// ---- util ----

// intis glfw & glad, also creates the window
// returns: <stddef.h> return_code
rtn_code create_window(const int width, const int height, const char* title, bee_bool maximized);

// set the windows title
void set_window_title(const char* title);

// set whether the window is maximized or not
void set_window_maximized(bee_bool maximize);

// get the current window
GLFWwindow* get_window();

// sets the given width and height ints to the windows size
void  get_window_size(int* width, int* height);

// get the windows current title
char* get_window_title();

// --- callbacks ----

// glfw error callback function
void error_callback(int error, const char* description);

// window resize callback
// resizes the "glViewport" according to the resized window
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

#endif
