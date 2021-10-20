#include <stdlib.h>
#include <direct.h>
#include <string.h>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
// #include "GLAD/glad.h"
// #include "CGLM/cglm.h"

#define  GLOBAL_H_IMPLEMENTATION
#include "global.h"
// #include "file_handler.h"
#include "asset_manager.h"
#include "game_time.h"
#include "editor_ui.h"
#include "renderer.h"
#include "entities.h"
#include "physics.h"
#include "window.h"
#include "input.h"
#include "app.h"

// #include "serialization.h"
// #include "scene_manager.h"

// temp
#include "shader.h"

// if defined build as editor, otherwise build as game
// #define EDITOR_ACT in project properties

f32 delta_t = 0.0f;	// Time between current frame and last frame
f32 last_frame = 0.0f;	// Time of last frame

f32 fps_t;
f32 cur_fps;
int fps_ticks_counter;


// ---- entry func ----
int main(void)
{

	assetm_init();
	printf(" -> assetm_init() finished\n");
	

	// all the boilerplate setup stuff
	if (create_window(1920, 1080, "bee engine", BEE_TRUE) == BEE_ERROR)
	{
		fprintf(stderr, "Initializing GLFW, GLAD or opening a window failed.\n exiting ... \n");
		assert(BEE_FALSE);
	}

	// add primitives & pink texture for missing texture & white material for added entities
	load_internal_assets();
	printf(" -> assetm load_internal_assets() finished\n");

	entities_init();
	printf(" -> entities_init() finished\n");
	init(); // @TODO: this should be last
	printf(" -> init() finished\n");
	gravity_ui_init();
	printf(" -> grav_ui_init() finished\n");
#ifdef EDITOR_ACT
	ui_init();
	printf(" -> ui_init() finished\n");
#endif
	input_init();
	printf(" -> input_init() finished\n");
	renderer_init();
	printf(" -> renderer_init() finished\n\n");

	// gui console 
	submit_txt_console("bee_engine :)");
	submit_txt_console("...");


	// main loop
	while (!glfwWindowShouldClose(get_window()))
	{
		// get and process events
		glfwPollEvents();

		// ---- time ----
		// printf("Time: %f \n", glfwGetTime());
		f32 currentFrame = (f32)glfwGetTime();
		delta_t = currentFrame - last_frame;
		last_frame = currentFrame;
		set_delta_time(delta_t);
		set_total_secs(currentFrame);
		// ---- fps -----
		fps_t += get_delta_time();
		fps_ticks_counter++;
		if (fps_t > 1.0f)
		{
			cur_fps = (int)floor((double)fps_ticks_counter / (double)fps_t);
			fps_ticks_counter = 0;
			fps_t = 0.0f;
			set_fps(cur_fps);
		}



		// call the update function in the apllication
		update();


		// ---- rendering ----
		renderer_update(); // render all objects

#ifdef EDITOR_ACT
		ui_update();
#endif

		entities_update(); // has to be after ui

		physics_update(); // after renderer & entities

		// reset last frames button state
		input_update();

		// ---- glfw stuff ----
		// swap back and front bufffer
		glfwSwapBuffers(get_window());
	}

	// free allocated memory
	cleanup();
#ifdef EDITOR_ACT
	ui_cleanup();
#endif
	renderer_cleanup();
	entities_cleanup();
	assetm_cleanup();

	// close window and terminate glfw
	glfwTerminate();

	return 0;
}




