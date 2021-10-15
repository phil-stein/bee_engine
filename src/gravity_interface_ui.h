#pragma once
#ifndef GRAVITY_INTERFACE_UI_H
#define GRAVITY_INTERFACE_UI_H

#include "gravity/gravity_compiler.h"
#include "gravity/gravity_macros.h"
#include "gravity/gravity_core.h"
#include "gravity/gravity_vm.h"
#include "gravity/gravity_vmmacros.h"

#include "global.h"
#include "gravity_script.h"

// creates the nuklear context
void gravity_ui_init();
// clears the nuklear context
void gravity_ui_clear();

//
// setup_ui_class() adds the UI class to a gravity virtual machine
// it can then be accessed in gravity: var ui = UI(); ui.function();
// 
// all other funtions are callbacks for gravity to call if the given 
// function is called inside a gravity file
// 


void setup_ui_class(gravity_vm* vm);

static bee_bool gravity_begin(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool gravity_end(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool gravity_window_begin(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool gravity_window_end(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool gravity_layout_row(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool gravity_layout_row_static(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool gravity_label(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool gravity_button(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool gravity_checkbox(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);
static bee_bool gravity_image(gravity_vm* vm, gravity_value_t* args, uint16_t nargs, uint32_t rindex);


void gravity_ui_set_style(struct nk_context* ctx, enum ui_theme theme);

#endif
