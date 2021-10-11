#pragma once
#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "global.h"


void create_framebuffer(u32* tex_buffer, u32* fbo, u32* rbo);
void create_framebuffer_hdr(u32* tex_buffer, u32* fbo, u32* rbo);
void create_framebuffer_multisampled(u32* tex_buffer, u32* fbo, u32* rbo);
void create_framebuffer_multisampled_hdr(u32* tex_buffer, u32* fbo, u32* rbo);
void create_framebuffer_shadowmap(u32* tex_buffer, u32* fbo, int width, int height);


void bind_framebuffer(u32 fbo);
void unbind_framebuffer();

void resize_frame_buffer_to_window(u32 tex_buffer);
void blit_multisampled_framebuffer(u32 fbo_msaa, u32 fbo);

#endif