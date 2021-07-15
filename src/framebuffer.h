#pragma once
#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "global.h"

void create_framebuffer(u32* tex_buffer);

void bind_framebuffer();
void unbind_framebuffer();

void resize_frame_buffer_to_window(u32* tex_buffer);

#endif