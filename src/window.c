#include <stdio.h>

#include "GLAD/glad.h"

#include "files/asset_manager.h"
#include "stb/stb_ds.h"
#include "editor_ui.h"
#include "window.h"

// ---- vars ----
GLFWwindow* window;
char* window_title;

platform current_platform;

bee_bool is_maximized = BEE_TRUE;;

// framebuffer ptr array to be resized on window size change
framebuffer** resize_buffers = NULL;
int			  resize_buffers_len = 0;

// intis glfw & glad, also creates the window
// returns: <stddef.h> return_code
rtn_code create_window(const int width, const int height, const char* title, bee_bool maximized)
{

	// enable error logging for glfw
	glfwSetErrorCallback(glfw_error_callback);

	// Initialise GLFW
	if (glfwInit() == GLFW_FALSE)
	{
		fprintf(stderr, "Failed to initialize GLFW !!!\n");
		return BEE_ERROR;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfwWindowHint(GLFW_SAMPLES, 4);
	// glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	// glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);

	// open a window and create its opengl context
	window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (window == NULL)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		return BEE_ERROR;
	}
	window_title = title;

	// make the window's context current
	glfwMakeContextCurrent(window);

	// initialize glad, load the opengl bindings
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		fprintf(stderr, "Failed to initialize GLAD");
		return BEE_ERROR;
	}

	// tell opengl the size of our window
	glViewport(0, 0, width, height);

	// set the resize callback
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	set_window_maximized(maximized);

	// callback for files dropped onto window
	glfwSetDropCallback(window, file_drop_callback);

	// ---- platform ----
#if (defined _WIN32 || defined _WIN64)
	
	#define WINDOWS
	current_platform = PLATFORM_WINDOWS;

#elif (defined linux || defined _linux || __linux__)

	#define LINUX
	current_platform = PLATFORM_LINUX;
	printf("[!!!] linux not yet properly supported.");

#elif (defined __APPLE__ || defined _APPLE)
	
	#define APPLE
	current_platform = PLATFORM_APPLE;
	printf("[!!!] apple mac_os not yet properly supported.");

#endif

	return BEE_OK; // all good :)
}

void close_window()
{
	glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void set_window_title(const char* title)
{
	glfwSetWindowTitle(window, title);
	window_title = (char*)title;
}

void set_window_maximized(bee_bool maximize)
{
	//glfwSetWindowAttrib(window, GLFW_MAXIMIZED, maximized == BEE_TRUE ? GLFW_TRUE : GLFW_FALSE);

	if (maximize == BEE_SWITCH)
	{
		set_window_maximized(!is_maximized);
		return;
	}

	if (maximize == BEE_TRUE)
	{
		glfwMaximizeWindow(window);
		is_maximized = BEE_TRUE;
	}
	else
	{
		glfwRestoreWindow(window);
		is_maximized = BEE_FALSE;
	}
}

// get the current window
GLFWwindow* get_window()
{
	return window;
}

void get_window_size(int* width, int* height)
{
	glfwGetWindowSize(window, width, height);
}

char* get_window_title() 
{
	return window_title;
}

bee_bool get_window_should_close()
{
	return (bee_bool)glfwWindowShouldClose(window);
}

void window_swap_buffers()
{
	glfwSwapBuffers(window);
}

void window_poll_events()
{
	glfwPollEvents(window);
}

void set_texturebuffer_to_update_to_screen_size(framebuffer* fb)
{
	arrput(resize_buffers, fb);
	resize_buffers_len++;
}

platform get_current_platform()
{
	return current_platform;
}

// ---- callbacks ----

void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "[GLFW-Error] %s\n", description);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	for (int i = 0; i < resize_buffers_len; ++i)
	{
		if (resize_buffers[i] == NULL) { continue; }
		resize_frame_buffer_to_window(resize_buffers[i]);
	}
}

void file_drop_callback(GLFWwindow* window, int path_count, const char* paths[])
{
	// only activate import ui when editor active
#ifdef EDITOR_ACT
	set_drag_and_drop_import_window(path_count, paths);
#endif
}