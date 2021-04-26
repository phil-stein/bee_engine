#include "global.h"
#include "native_script.h"


// function declarations
script make();
void _init();
void _update();
void _cleanup();

script make()
{
	script _script;
	_script.init	= &_init;
	_script.update	= &_update;
	_script.cleanup = &_cleanup;

	return _script;
}

void _init()
{

}

void _update()
{

}

void _cleanup()
{

}