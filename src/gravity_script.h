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
	char* source;

	int entity_index;

	gravity_vm* vm;

	gravity_closure_t* closure;
	bee_bool update_closure_assigned;

	bee_bool active;

}gravity_script;

gravity_script make_script(char* path);
void free_script(gravity_script* script);

rtn_code gravity_run(const char* source_code);
rtn_code gravity_run_init(gravity_script* script, const char* src);
rtn_code gravity_run_update(gravity_script* script);
rtn_code gravity_run_cleanup(gravity_script* script);



void setup_entity_class(gravity_vm* vm);

static bee_bool move_ent_x(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool move_ent_y(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool move_ent_z(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);

static bee_bool get_ent_x(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_ent_y(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_ent_z(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);


void setup_game_class(gravity_vm* vm);

static bee_bool get_game_total_sec(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_game_delta_t(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);


void setup_input_class(gravity_vm* vm);

static bee_bool get_key_W(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_A(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_S(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_D(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_ARROW_LEFT(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_ARROW_RIGHT(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_ARROW_UP(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_ARROW_DOWN(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_SPACE(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);

#endif

