#include "game_time.h"

#include "GLFW/glfw3.h"


f32 delta_time = 0.0f; // time between current frame and last frame
f32 last_frame = 0.0f; // time of last frame

// time since application started
f32 total_secs = 0.0f;

f32 frames_per_second;
f32 fps_t;
int fps_ticks_counter;


void game_time_update()
{

	// ---- delta t ----
	f32 currentFrame = (f32)glfwGetTime();
	delta_time = currentFrame - last_frame;
	last_frame = currentFrame;

	// ---- fps -----
	fps_t += delta_time;
	fps_ticks_counter++;
	if (fps_t > 1.0f)
	{
		frames_per_second = (int)floor((f64)fps_ticks_counter / (f64)fps_t);
		fps_ticks_counter = 0;
		fps_t = 0.0f;
	}
}


f32 get_delta_time()
{
	return delta_time;
}

f32 get_total_secs() 
{
	return (f32)glfwGetTime();
}

f32 get_fps()
{
	return frames_per_second;
}
