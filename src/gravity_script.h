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
static bee_bool game_quit(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);


void setup_world_class(gravity_vm* vm);

static bee_bool world_get_entity(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);

static bee_bool world_move_ent_x(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool world_move_ent_y(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool world_move_ent_z(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);

static bee_bool world_get_ent_x(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool world_get_ent_y(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool world_get_ent_z(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);


void setup_input_class(gravity_vm* vm);

static bee_bool get_key_SPACE(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_APOSTROPHE(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_COMMA(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_MINUS(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_PERIOD(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_SLASH(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_0(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_1(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_2(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_3(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_4(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_5(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_6(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_7(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_8(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_9(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_SEMICOLON(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_EQUAL(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_A(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_B(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_C(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_D(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_E(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_F(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_G(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_H(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_I(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_J(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_K(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_L(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_M(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_N(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_O(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_P(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_Q(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_R(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_S(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_T(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_U(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_V(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_W(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_X(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_Y(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_Z(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_LEFT_BRACKET(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_BACKSLASH(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_RIGHT_BRACKET(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_GRAVE_ACCENT(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_WORLD_1(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_WORLD_2(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_ESCAPE(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_ENTER(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_TAB(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_BACKSPACE(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_INSERT(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_ARROW_LEFT(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_ARROW_RIGHT(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_ARROW_UP(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_ARROW_DOWN(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_DELETE(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_PAGE_UP(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_PAGE_DOWN(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_HOME(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_END(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_CAPS_LOCK(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_SCROLL_LOCK(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_NUM_LOCK(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_PRINT(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_PAUSE(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);


void setup_camera_class(gravity_vm* vm);

static bee_bool move_cam_x(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool move_cam_y(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool move_cam_z(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_cam_x(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_cam_y(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_cam_z(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);


#endif

