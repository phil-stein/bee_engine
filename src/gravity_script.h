#pragma once
#ifndef GRAVITY_SCRIPT_H
#define GRAVITY_SCRIPT_H

#include "gravity/gravity_compiler.h"
#include "gravity/gravity_macros.h"
#include "gravity/gravity_core.h"
#include "gravity/gravity_vm.h"
#include "gravity/gravity_vmmacros.h"

#include "global.h"


typedef struct
{
	char* path;
	bee_bool path_valid;
	char* source;

	int entity_index;

	gravity_vm* vm;

	gravity_closure_t* closure;
	bee_bool update_closure_assigned;

	bee_bool active;
	bee_bool has_error;

}gravity_script;

// void report_error(gravity_vm* vm, error_type_t error_type, const char* message, error_desc_t error_desc, void* xdata);
void throw_error(char* msg);

gravity_script make_script(char* path);
void free_script(gravity_script* script);

rtn_code gravity_run(const char* source_code);
rtn_code gravity_run_init(gravity_script* script, const char* src);
rtn_code gravity_run_update(gravity_script* script);
rtn_code gravity_run_cleanup(gravity_script* script);


#endif

