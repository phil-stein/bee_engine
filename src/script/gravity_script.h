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
	char* name;
	char* path;
	bee_bool path_valid;
	char* source;

	// int entity_index;

	gravity_vm* vm;

	gravity_closure_t* closure;
	bee_bool init_closure_assigned;
	bee_bool update_closure_assigned;
	bee_bool trigger_closure_assigned;
	bee_bool collision_closure_assigned;

	bee_bool active;
	bee_bool has_on_trigger;
	bee_bool has_on_collision;
	bee_bool has_error;

}gravity_script;

// void report_error(gravity_vm* vm, error_type_t error_type, const char* message, error_desc_t error_desc, void* xdata);
void throw_error(char* msg);
// for 'System.print()' calls
void print_callback(gravity_vm* vm, const char* message, void* xdata);
// for including files
const char* load_file(const char* file, size_t* size, uint32_t* fileid, void* xdata, int* is_static);

gravity_script make_script(char* path);
void free_script(gravity_script* script);

rtn_code gravity_run_init(gravity_script* script, const char* src, int entity_index);
rtn_code gravity_run_update(gravity_script* script, int entity_index);
rtn_code gravity_run_on_trigger(gravity_script* script, int entity_index, int collision_id);
rtn_code gravity_run_on_collision(gravity_script* script, int entity_index, int collision_id);

#endif

