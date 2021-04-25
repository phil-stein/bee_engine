#include <stdio.h>

#include "GLAD/glad.h"

#include "window.h"

// ---- vars ----
GLFWwindow* window;
char* window_title;

// intis glfw & glad, also creates the window
// returns: <stddef.h> return_code
rtn_code create_window(const int width, const int height, const char* title, bee_bool maximized)
{

	// enable error logging for glfw
	glfwSetErrorCallback(error_callback);

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

	return BEE_OK; // all good :)
}

void set_window_title(const char* title)
{
	glfwSetWindowTitle(window, title);
	window_title = (char*)title;
}

void set_window_maximized(bee_bool maximize)
{
	//glfwSetWindowAttrib(window, GLFW_MAXIMIZED, maximized == BEE_TRUE ? GLFW_TRUE : GLFW_FALSE);

	if (maximize == BEE_TRUE)
	{
		glfwMaximizeWindow(window);
	}
	else
	{
		glfwRestoreWindow(window);
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

// ---- callbacks ----

// glfw error func
void error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW-Error: %s\n", description);
}
// window resize callback
// resizes the "glViewport" according to the resized window
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}