#pragma once
#ifndef WINDOW_H
#define WINDOW_H

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include "types/framebuffer.h"
#include "global.h"

// the engine also defines "WINDOWS", "LINUX" or "APPLE", to check that way
// !!! not all platforms in enum supported yet
typedef enum platform { PLATFORM_WINDOWS, PLATFORM_LINUX, PLATFORM_APPLE }platform;


// intis glfw & glad (opengl), also creates the window
// returns: "global.h" return_code
rtn_code create_window(const int width, const int height, const char* title, bee_bool maximized);

// sets the variable returned by "get_window_should_close()" to true, causing the application to exit the main loop, i.e. quit
void close_window();

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

// true:  window should be closed i.e. exit main loop and call glfwTerminate()
// false: all good keep the application running
bee_bool get_window_should_close();

// swap the front and back buffers glfw created for us when creating a window
// the back buffer is the one we render to and the front buffer is the one being displayed
// swapping them shows the new rendered frame on screen
void window_swap_buffers();

// tell glfw to process events that stacked up since last call to this function
// events: mouse-movement, key-presses, file dropped over window, etc.
// when this function isnt called regularly the application is tagged unresponsive by the os
void window_poll_events();

// register a texture buffer to be resized if the window size changes
void set_texturebuffer_to_update_to_screen_size(framebuffer* fb);

// get the platform the program currently is running on / compiled for
// the engine also defines "WINDOWS", "LINUX" or "APPLE", to check that way
// !!! only returns valid after "create_window()" has been called
platform get_current_platform();

// --- callbacks ----

// glfw error callback function
void glfw_error_callback(int error, const char* description);

// window resize callback
// resizes the "glViewport" according to the resized window
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// files dropped onto window callback
void file_drop_callback(GLFWwindow* window, int path_count, const char* paths[]);

#endif
