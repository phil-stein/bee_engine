#include <stdlib.h>
#include <direct.h>
#include <string.h>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
// #include "GLAD/glad.h"
// #include "CGLM/cglm.h"

#include "global.h"
// #include "file_handler.h"
#include "renderer.h"
#include "asset_manager.h"
#include "game_time.h"
#include "window.h"
#include "input.h"
#include "app.h"
#include "ui.h"

// temp
#include "shader.h"

// if defined build as editor, otherwise build as game
// #define EDITOR_ACT in project properties

f32 delta_t = 0.0f;	// Time between current frame and last frame
f32 last_frame = 0.0f;	// Time of last frame

// ---- entry func ----
int main(void)
{

	assetm_init();
	printf(" -> assetm_init() finished\n");
	// initialize the application
	init(); // @TODO: this should be last
	printf(" -> init() finished\n");
	ui_init();
	printf(" -> ui_init() finished\n");
	input_init();
	printf(" -> input_init() finished\n");
	renderer_init();
	printf(" -> renderer_init() finished\n\n");

	// gui console test 
	submit_txt_console("console messages working :)");
	submit_txt_console("djfkvndfkjvhndifvhnjydlvhnldihfbvdfijbvhnpijubdhfvdihubvuihi");

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


		// call the update function in the apllication
		update();


		// ---- rendering ----
		renderer_update(); // render all objects


#ifdef EDITOR_ACT
		ui_update();
#endif

		// reset last frames button state
		input_update();

		// ---- glfw stuff ----
		// swap back and front bufffer
		glfwSwapBuffers(get_window());
	}

	// free allocated memory
	cleanup();
	ui_cleanup();
	renderer_cleanup();
	assetm_cleanup();

	// close window and terminate glfw
	glfwTerminate();

	return 0;
}




