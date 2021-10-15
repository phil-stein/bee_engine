#ifndef GLOBAL_H
#define GLOBAL_H

// need this basically everywhere
#include <assert.h>
#include <stdio.h>


// define NULL as a void pointer
// pointer value pointing to nothing
#ifndef NULL	
#define NULL ((void *)0)
#endif

enum rtn_code {
	BEE_ERROR	= 0,
	BEE_OK		= 1
};
// define custom return checks
// not for use when programming, just for verbosity
typedef enum rtn_code rtn_code;

enum bee_bool {
	BEE_FALSE	= 0,
	BEE_TRUE	= 1,
	BEE_SWITCH  = 2  // passed as argument, f.e. set(bee_bool act) passing BEE_SWITCH to switch the state 
};
// define customs return checks
typedef enum bee_bool bee_bool;

// error types mainly used for "set_error_popup()" in editor_ui.h
typedef enum error_type
{
	GENERAL_ERROR,
	GRAVITY_ERROR
}error_type;

// ints
typedef signed char			s8;		// signed 8 bit integer
typedef short				s16;	// signed 16 bit integer
typedef int					s32;	// signed 32 bit integer
typedef long long			s64;	// signed 64 bit integer
typedef unsigned char		u8;		// unsigned 8 bit integer
typedef unsigned short		u16;	// unsigned 16 bit integer
typedef unsigned int		u32;	// unsigned 32 bit integer
typedef unsigned long long	u64;	// unsigned 64 bit integer

// floats
typedef float				f32;	// 32 bit floating point number
typedef double				f64;	// 64 bit floating point number


// void function template with no args, used for calllbacks
typedef void (*empty_callback)();

#endif