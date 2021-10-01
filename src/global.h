#ifndef GLOBAL_H
#define GLOBAL_H

// need this basically everywhere
#include <assert.h>
#include <stdio.h>


// define NULL as a void pointer
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

// define customs return checks
enum bee_bool {
	BEE_FALSE	= 0,
	BEE_TRUE	= 1,
	BEE_SWITCH  = 2  // passed as argument, f.e. set(bee_bool act) passing BEE_SWITCH to switch the state 
};
typedef enum bee_bool bee_bool;

// ints
typedef signed char			s8;
typedef short				s16;
typedef int					s32;
typedef long long			s64;
typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned int		u32;
typedef unsigned long long	u64;

// floats
typedef float				f32;
typedef double				f64;

#endif