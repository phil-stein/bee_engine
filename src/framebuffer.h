#pragma once
#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "global.h"

void create_framebuffer(u32* tex_buffer);
void create_framebuffer_multisampled(u32* tex_buffer);

void bind_framebuffer(bee_bool multisampled);
void unbind_framebuffer();

void resize_frame_buffer_to_window(u32* tex_buffer);

void blit_multisampled_framebuffer();

#endif