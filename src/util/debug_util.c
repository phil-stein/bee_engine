#include "debug_util.h"

#include "stb/stb_ds.h" // STB_DS_IMPLEMENTATOn defined in main.c

#include "game_time.h"
#include "util/str_util.h"

#ifdef DEBUG_UTIL_ACT

timer* timer_stack = NULL;	// stack of all active timers
int timer_stack_len = 0;	// ^ length

timer* cur_state_timer_stack = NULL; // array with the states of the timers popped last frame
int cur_state_timer_stack_len = 0;	 // ^ length
timer* state_timer_stack = NULL; // array with the states of the timers popped last frame
int state_timer_stack_len = 0;	 // ^ length

void start_timer_func(char* name, char* file, char* func)
{
	timer t;
	t.time = (f64)get_total_secs();
	t.name = name;

	char* file_name = str_find_last_of(file, "\\");
	file_name++; // the first char is the '\'
	t.file = file_name;

	t.func = func;

	arrpush(timer_stack, t);
	timer_stack_len++;
}

bee_bool can_stop_timer()
{
	return timer_stack_len > 0;
}

timer stop_timer_func()
{
	if (timer_stack_len <= 0) { timer t; t.name = "x"; t.time = 0.0; return t; }

	// f64 t = arrpop(timer_stack);
	timer t = timer_stack[timer_stack_len - 1]; // get last elem
	arrdel(timer_stack, timer_stack_len -1);  // delete last elem
	t.time = get_total_secs() - t.time; // get the delta t
	t.time *= 1000; // bring to millisecond range
	timer_stack_len--;

	// timer state
	arrput(cur_state_timer_stack, t);
	cur_state_timer_stack_len++;

	return t;
}

f64  stop_timer_print_func()
{
	timer t = stop_timer_func();
	printf("[TIMER] | %s | %.2Lfms\n", t.name, t.time);
	return t.time;
}

timer* get_all_timers(int* len)
{
	*len = state_timer_stack_len;
	return state_timer_stack;
}

void clear_state_timers()
{
	// switch last / current frame timer state stack
	timer* tmp				  = state_timer_stack;
	int    tmp_len			  = state_timer_stack_len;
	state_timer_stack		  = cur_state_timer_stack;
	state_timer_stack_len	  = cur_state_timer_stack_len;
	cur_state_timer_stack	  = tmp;
	cur_state_timer_stack_len = tmp_len;

	arrfree(cur_state_timer_stack);
	cur_state_timer_stack = NULL;
	cur_state_timer_stack_len = 0;
}

#else

void start_timer_func(char* name, char* file, char* func)
{ }

bee_bool can_stop_timer()
{ }

timer stop_timer_func()
{
	timer t; t.time = 0.0; t.name = "x"; t.file = "x"; t.func = "x"; return t;
}

f64 stop_timer_print_func()
{ return 0.0; }

timer* get_all_timers(int* len)
{
	*len = 0;
	return NULL;
}

void clear_state_timers()
{ }

#endif;