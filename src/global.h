#ifndef GLOBAL_H
#define GLOBAL_H

// need this basically everywhere
#include <assert.h>
#include <stdio.h>
#include <corecrt_math.h>


// "global.h" is included everywhere, so this will def / undef globally
// #define EDITOR_ACT
// #undef  EDITOR_ACT


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
	GENERAL_ERROR,	// non specific of error, usually use this
	GRAVITY_ERROR	// error in the gravity scripting language
}error_type;

// ints
typedef signed char			s8;		// signed 8 bit integer | signed char
typedef short				s16;	// signed 16 bit integer | short
typedef int					s32;	// signed 32 bit integer | int
typedef long long			s64;	// signed 64 bit integer | long long
typedef unsigned char		u8;		// unsigned 8 bit integer | unsigned char
typedef unsigned short		u16;	// unsigned 16 bit integer | unsigned short
typedef unsigned int		u32;	// unsigned 32 bit integer | unsigned int
typedef unsigned long long	u64;	// unsigned 64 bit integer | unsigned long long

// floats
typedef float				f32;	// 32 bit floating point number | float
typedef double				f64;	// 64 bit floating point number | double


// void function template with no args, used for calllbacks
typedef void (*empty_callback)();


#ifdef GLOBAL_H_IMPLEMENTATION // defined in main.c

// 
// util ------------------------------------------------------------------
// 

// magnitude / length of vec3, didnt find a glm function
f32 vec3_magnitude(f32 v[3])
{
	return sqrtf((v[0] * v[0]) +
				 (v[1] * v[1]) +
				 (v[2] * v[2]));
}

// multiplies vec3 with float
void vec3_mul_f(f32 v[3], f32 f)
{
	v[0] *= f; v[1] *= f; v[2] *= f;
}

void vec3_add_f(f32 v[3], f32 f)
{
	v[0] += f; v[1] += f; v[2] += f;
}
#else
// magnitude / length of vec3, didnt find a glm function
f32 vec3_magnitude(f32 v[3]);
#define vec3_length(v) vec3_magnitude(v)

// multiplies vec3 with float
void vec3_mul_f(f32 v[3], f32 f);

// add float to x,y,z of vec3
void vec3_add_f(f32 v[3], f32 f);
#endif

#endif