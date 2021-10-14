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


void gravity_ui_init();


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


void gravity_ui_set_style(struct nk_context* ctx, enum theme theme);

#endif
