#pragma once
#ifndef GRAVITY_INTERFACE_H
#define GRAVITY_INTERFACE_H

#include "gravity/gravity_compiler.h"
#include "gravity/gravity_macros.h"
#include "gravity/gravity_core.h"
#include "gravity/gravity_vm.h"
#include "gravity/gravity_vmmacros.h"

#include "global.h"
#include "gravity_script.h"


// set the currently active script
void set_cur_script(gravity_script* script, int entity_index);
// get the currently active script
gravity_script* get_cur_script();

// actions that cannot be executed while the gravity scripts are being executed get executed here afterwards
void gravity_check_for_pending_actions();
// reset the state of all actions to false
void gravity_reset_pending_actions();
// mouse-movement callback used for input related fuctions
void mouse_movement_callback();

//
// setup_..._class() functions add the given class to a gravity virtual machine
// they then can be accessed in gravity: var class = Class(); class.function();
// 
// all other funtions are callbacks for gravity to call if the given function is
// called inside a gravity file
// 


void setup_entity_class(gravity_vm* vm);

static bee_bool move_ent_x(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool move_ent_y(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool move_ent_z(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);

static bee_bool get_ent_x(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_ent_y(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_ent_z(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);

static bee_bool rot_ent_x(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool rot_ent_y(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool rot_ent_z(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);

static bee_bool get_ent_x_rot(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_ent_y_rot(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_ent_z_rot(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);

void setup_game_class(gravity_vm* vm);

static bee_bool get_game_total_sec(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_game_delta_t(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool game_quit(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool game_load_level(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool game_console(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool game_error(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool game_add_mesh(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);

void setup_world_class(gravity_vm* vm);

static bee_bool world_add_ent(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);

static bee_bool world_get_entity(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool world_get_camera(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);

static bee_bool world_move_ent_x(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool world_move_ent_y(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool world_move_ent_z(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);

static bee_bool world_get_ent_x(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool world_get_ent_y(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool world_get_ent_z(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);

static bee_bool world_rot_ent_x(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool world_rot_ent_y(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool world_rot_ent_z(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);

static bee_bool world_get_ent_x_rot(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool world_get_ent_y_rot(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool world_get_ent_z_rot(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);

static bee_bool world_scale_ent_x(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool world_scale_ent_y(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool world_scale_ent_z(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);

static bee_bool world_get_ent_x_scale(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool world_get_ent_y_scale(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool world_get_ent_z_scale(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);


void setup_input_class(gravity_vm* vm);

static bee_bool input_set_cursor(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool input_rotate_cam_by_mouse(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool input_stop_rotate_cam_by_mouse(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);

// get_key()
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
static bee_bool get_key_F1(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_F2(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_F3(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_F4(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_F5(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_F6(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_F7(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_F8(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_F9(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_F10(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_F11(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_F12(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_F12(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
// ...
static bee_bool get_key_DECIMAL(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_DIVIDE(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_MULTIPLY(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_ADD(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_SHIFT(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_CONTROL(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_ALT(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_SUPER(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_WIN_MAC_SYMBOL(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_MENU(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);

// get_key_down()
static bee_bool get_key_down_SPACE(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_APOSTROPHE(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_COMMA(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_MINUS(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_PERIOD(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_SLASH(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_0(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_1(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_2(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_3(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_4(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_5(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_6(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_7(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_8(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_9(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_SEMICOLON(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_EQUAL(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_A(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_B(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_C(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_D(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_E(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_F(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_G(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_H(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_I(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_J(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_K(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_L(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_M(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_N(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_O(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_P(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_Q(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_R(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_S(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_T(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_U(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_V(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_W(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_X(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_Y(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_Z(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_LEFT_BRACKET(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_BACKSLASH(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_RIGHT_BRACKET(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_GRAVE_ACCENT(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_WORLD_1(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_WORLD_2(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_ESCAPE(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_ENTER(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_TAB(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_BACKSPACE(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_INSERT(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_ARROW_LEFT(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_ARROW_RIGHT(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_ARROW_UP(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_ARROW_DOWN(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_DELETE(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_PAGE_UP(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_PAGE_DOWN(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_HOME(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_END(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_CAPS_LOCK(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_SCROLL_LOCK(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_NUM_LOCK(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_PRINT(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_PAUSE(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_F1(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_F2(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_F3(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_F4(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_F5(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_F6(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_F7(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_F8(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_F9(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_F10(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_F11(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_F12(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_F12(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
// ...
static bee_bool get_key_down_DECIMAL(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_DIVIDE(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_MULTIPLY(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_ADD(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_SHIFT(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_CONTROL(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_ALT(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_SUPER(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_WIN_MAC_SYMBOL(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool get_key_down_MENU(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);


// void setup_camera_class(gravity_vm* vm);

// static bee_bool move_cam_x(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
// static bee_bool move_cam_y(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
// static bee_bool move_cam_z(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
// static bee_bool get_cam_x(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
// static bee_bool get_cam_y(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
// static bee_bool get_cam_z(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);


#endif
