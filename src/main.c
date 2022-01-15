#include <stdlib.h>
#include <direct.h>
#include <string.h>

// #define GLFW_INCLUDE_NONE
// #include "GLFW/glfw3.h"
// #include "GLAD/glad.h"
// #include "CGLM/cglm.h"

#define  GLOBAL_H_IMPLEMENTATION
#include "global.h"

#include "script/gravity_interface_ui.h"
#include "files/asset_manager.h"
#include "physics/physics.h"
#include "types/entities.h"
#include "util/debug_util.h"
#include "game_time.h"
#include "editor_ui.h"
#include "renderer.h"
#include "window.h"
#include "input.h"
#include "app.h"



// ---- entry func ----
int main(void)
{
	TIMER_START("---- init ----");

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

	set_bg_till_loaded();

	entities_init();
	printf(" -> entities_init() finished\n");
	gravity_ui_init();
	printf(" -> gravity_ui_init() finished\n");
	#ifdef EDITOR_ACT
	ui_init();
	printf(" -> ui_init() finished\n");
	#endif
	renderer_init();
	printf(" -> renderer_init() finished\n");
	input_init();
	printf(" -> input_init() finished\n");
	physics_init();
	printf(" -> physics_init() finished\n\n");
	app_init(); // @TODO: this should be last
	printf(" -> app_init() finished\n");

	TIMER_STOP_PRINT(); // init

	// gui console 
	submit_txt_console("bee_engine :)");
	submit_txt_console("...");


	// main loop
	while (!get_window_should_close())
	{
		TIMER_START("---- frame ----"); // whole frame

		// get and process events
		window_poll_events();

		// ---- time ----
		game_time_update();


		// call the update function in the apllication
		TIMER_START("app_update()");
		app_update();
		TIMER_STOP();

		TIMER_START("renderer_update()");
		renderer_update(); // render all objects
		TIMER_STOP();


		#ifdef EDITOR_ACT
		TIMER_START("ui_update()");
		ui_update();
		TIMER_STOP();
		#endif

		TIMER_START("entities_update()");
		entities_update(); // has to be after ui
		TIMER_STOP();

		// only need to / can check gamestate when editor is active
		TIMER_START("physics_update()");
		#ifdef EDITOR_ACT
		if (get_gamestate()) // only wile playing
		{
			physics_update(get_delta_time()); // after renderer & entities
		}
		#else 
			physics_update(get_delta_time()); // after renderer & entities
		#endif
		TIMER_STOP(); // physics update

		TIMER_START("input_update()");
		input_update(); // reset last frames button state
		TIMER_STOP();

		// ---- glfw stuff ----
		// swap back and front bufffer
		window_swap_buffers();

		TIMER_STOP(); // frame

		clear_state_timers();
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




