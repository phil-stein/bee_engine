#pragma once
#ifndef DEBUG_UTIL_H
#define DEBUG_UTIL_H

#include "global.h"

typedef struct timer
{
	f64 time;	// time between "timer_start()" and "timer_stop()" in ms
	char* name;	// name of the timer, given in "timer_start()"
	char* file;	// name of the file in which "timer_start()" was called
	char* func;	// name of the function in which "timer_start()" was called
}timer;

#ifdef DEBUG_UTIL_ACT

// start a timer, pushing a new one onto the stack,
// call either "STOP_TIMER()" or "STOP_TIMER_PRINT()" to pop / stop timer
void   start_timer_func(char* name, char* file, char* func);

// start a timer, pushing a new one onto the stack,
// call either "STOP_TIMER()" or "STOP_TIMER_PRINT()" to pop / stop timer
#define TIMER_START(n) start_timer_func(n, __FILE__, __FUNCTION__)

// check whether there is a timer on the stack to be stopped
bee_bool can_stop_timer();

// pops timer of stack, stopping it, also calculates the "timer.time" value
timer  stop_timer_func();

// pops timer of stack, stopping it, also calculates the "timer.time" value
#define TIMER_STOP() if (!can_stop_timer()) { P_ERR("attempted to stop timer without starting one first."); } stop_timer_func()

// stops timer / pops it off stack and prints a message based on the name given in "START_TIMER()"
f64    stop_timer_print_func();

// stops timer / pops it off stack and prints a message based on the name given in "START_TIMER()"
#define TIMER_STOP_PRINT() if (!can_stop_timer()) { P_ERR("attempted to stop timer without starting one first."); } stop_timer_print_func()

// get the array/stack of current timers
timer* get_all_timers(int* len);

// !!! only used internally !!!
void clear_state_timers();

#else

// functions / macros in "debug_utils.h" are deactivated
// to activate them define "DEBUG_UTIL_ACT" globally, f.e. in "global.h" or your ide
void start_timer_func(char* name, char* file, char* func);

// functions / macros in "debug_utils.h" are deactivated
// to activate them define "DEBUG_UTIL_ACT" globally, f.e. in "global.h" or your ide
#define TIMER_START(n)

// functions / macros in "debug_utils.h" are deactivated
// to activate them define "DEBUG_UTIL_ACT" globally, f.e. in "global.h" or your ide
bee_bool can_stop_timer();

// functions / macros in "debug_utils.h" are deactivated
// to activate them define "DEBUG_UTIL_ACT" globally, f.e. in "global.h" or your ide
timer  stop_timer_func();

// functions / macros in "debug_utils.h" are deactivated
// to activate them define "DEBUG_UTIL_ACT" globally, f.e. in "global.h" or your ide
#define TIMER_STOP()

// functions / macros in "debug_utils.h" are deactivated
// to activate them define "DEBUG_UTIL_ACT" globally, f.e. in "global.h" or your ide
f64 stop_timer_print_func();

// functions / macros in "debug_utils.h" are deactivated
// to activate them define "DEBUG_UTIL_ACT" globally, f.e. in "global.h" or your ide
#define TIMER_STOP_PRINT()

// functions / macros in "debug_utils.h" are deactivated
// to activate them define "DEBUG_UTIL_ACT" globally, f.e. in "global.h" or your ide
timer* get_all_timers(int* len);

// functions / macros in "debug_utils.h" are deactivated
// to activate them define "DEBUG_UTIL_ACT" globally, f.e. in "global.h" or your ide
void clear_state_timers();

#endif

#endif