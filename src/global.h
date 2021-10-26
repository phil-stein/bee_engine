#ifndef GLOBAL_H
#define GLOBAL_H

// need this basically everywhere--------------------------------
#include <assert.h>
#include <stdio.h>
#include <corecrt_math.h>
// --------------------------------------------------------------


// "global.h" is included everywhere, so this will def globaly
// #define EDITOR_ACT



// notepad ---------------------------------------------------
// 
// printf():
//			"%d/i" int, "%u"  uint, "%ld/li" long int,
//			"%lu" long uint
//			"%f" float, "%Lf" double
//			"%c" char, "%s" string, "%p" pointer
// 
//			"%.nf" float max n decimal numbers
//			"%.ni" int   min n numbers
// 
// -----------------------------------------------------------
#define NOTEPAD
// defined so the text above shows up as description






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
	GENERAL_ERROR,		// non specific of error, usually use this
	GRAVITY_ERROR,		// error in the gravity scripting language
	GRAVITY_CALL_ERROR	// error called from inside the gravity scripting language
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

// 
// util / helpers --------------------------------------------------------
// 

// define NULL as a void pointer
// pointer value pointing to nothing
#ifndef NULL	
#define NULL ((void *)0)
#endif


#ifdef GLOBAL_H_IMPLEMENTATION // defined in main.c


f32 vec3_magnitude(f32 v[3])
{
	return sqrtf((v[0] * v[0]) +
				 (v[1] * v[1]) +
				 (v[2] * v[2]));
}

void vec3_mul_f(f32 v[3], f32 f)
{
	v[0] *= f; v[1] *= f; v[2] *= f;
}
void vec3_add_f(f32 v[3], f32 f)
{
	v[0] += f; v[1] += f; v[2] += f;
}

// void extract_rotation_from_matrix(mat4 rot_m, vec3 rot)
// {
// 	// ...
// }

// other ------------------------------------------

bee_bool get_editor_act()
{
	#ifdef EDITOR_ACT
	return BEE_TRUE;
	#else
	return BEE_FALSE;
	#endif
}

f32 rnd_num()
{
	// limits the random int returned by rand to a 0.0 - 1.0 range
	return (f32)rand() / RAND_MAX;
}

#else

// other ------------------------------------------

// returns: true/1 if compiled with editor, false/0 if not
bee_bool get_editor_act();

// @TODO: make a random generation util file
// returns: random float with range 0.0 - 1.0
f32 rnd_num();

// -------------------------------------------------

// printf helpers ----------------------------------

#define P_INT(i)	 printf("int: %i\n", i)		// printf integer value, -> "int: i"
#define P_INT64(i)   printf("int64: %li\n", i)	// printf long long value, -> "int64: i"
#define P_UINT(u)	 printf("uint: %u\n", u)	// printf unsigned int value, -> "uint: u"
#define P_UINT64(u)	 printf("uint64: %lu\n", u)	// printf unsigned long long value, -> "uint64: u"
#define P_F32(f)	 printf("f32: %f\n", f)		// printf float value, -> "f32: f"
#define P_F64(f)	 printf("f64: %Lf\n", f)	// printf double value, -> "f64: f"

#define P_CHAR(c)	 printf("char: %c\n", c)	// printf char, -> "char: c"
#define P_STR(s)	 printf("str: %s\n", s)		// printf char array, -> "str: s"
#define P_PTR(p)	 printf("ptr: %p\n", p)		// printf pointer address, -> "ptr: p"

// -------------------------------------------------

// cglm helpers ------------------------------------

// values
#define VEC3_X    (vec3){ 1.0f, 0.0f, 0.0f } // use: func(VEC3_X);
#define VEC3_Y    (vec3){ 0.0f, 1.0f, 0.0f } // use: func(VEC3_Y);
#define VEC3_Z    (vec3){ 0.0f, 0.0f, 1.0f } // use: func(VEC3_Z);
#define VEC3_ONE  (vec3){ 1.0f, 1.0f, 1.0f } // use: func(VEC3_ONE);
#define VEC3_ZERO (vec3){ 0.0f, 0.0f, 0.0f } // use: func(VEC3_ZERO);

// initializers
#define VEC3_X_INIT    { 1.0f, 0.0f, 0.0f } // use: vec3 v = VEC3_X_INIT;
#define VEC3_Y_INIT    { 0.0f, 1.0f, 0.0f }	// use: vec3 v = VEC3_Y_INIT;
#define VEC3_Z_INIT    { 0.0f, 0.0f, 1.0f }	// use: vec3 v = VEC3_Z_INIT;
#define VEC3_ONE_INIT  { 1.0f, 1.0f, 1.0f }	// use: vec3 v = VEC3_ONE_INIT;
#define VEC3_ZERO_INIT { 0.0f, 0.0f, 0.0f }	// use: vec3 v = VEC3_ZERO_INIT;

// magnitude / length of vec3, didnt find a glm function
f32 vec3_magnitude(f32 v[3]);
#define vec3_length(v) vec3_magnitude(v)

// multiplies vec3 with float
void vec3_mul_f(f32 v[3], f32 f);

// add float to x,y,z of vec3
void vec3_add_f(f32 v[3], f32 f);

// TODO: not implemented
// extracts the x, y, z rotation values from affine matrix in degrees
// void extract_rotation_from_matrix(mat4 rot_m, vec3 rot);

// -------------------------------------------------

#endif

#endif