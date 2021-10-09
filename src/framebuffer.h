#pragma once
#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "global.h"

typedef enum framebuffer_type { COLOR_BUFFER, MSAA_BUFFER, SHADOW_BUFFER }framebuffer_type;

void create_framebuffer(u32* tex_buffer);
void create_framebuffer_multisampled(u32* tex_buffer);
void create_framebuffer_shadowmap(u32* tex_buffer, u32* fbo, int width, int height);

void bind_framebuffer_type(framebuffer_type type);
void bind_framebuffer(u32 fbo);
void unbind_framebuffer();

void resize_frame_buffer_to_window(u32* tex_buffer);

void blit_multisampled_framebuffer();

#endif