#include "game_time.h"

// time between current frame and last frame
f32 delta_time = 0.0f;

// time since application started
f32 total_secs = 0.0f;

f32 get_delta_time()
{
	return delta_time;
}
void set_delta_time(f32 delta_t)
{
	delta_time = delta_t;
}

f32 get_total_secs() 
{
	return total_secs;
}
void set_total_secs(f32 secs)
{
	total_secs = secs;
}
