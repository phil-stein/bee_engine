#pragma once
#ifndef NATIVE_SCRIPT_H
#define NATIVE_SCRIPT_H

typedef struct
{
	// @UNCLEAR: this is is very oop, do I want this ???
	// in the add scipt function add these pointers to some array so they can be called at the appropriate time
	void (*init)   (void);
	void (*update) (void);
	void (*cleanup)(void);

}script;

#endif

