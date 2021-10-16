#pragma once
#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "global.h"


// create a basic RGB framebuffer with depth(24bits) and stencil(8bits) buffer attached
void create_framebuffer(u32* tex_buffer, u32* fbo, u32* rbo);
// create a hdr RGB16F framebuffer with depth(24bits) and stencil(8bits) buffer attached
// RGB16F: 16bit float per red, green and blue channel
void create_framebuffer_hdr(u32* tex_buffer, u32* fbo, u32* rbo);
// create a multisampled RGB framebuffer with depth(24bits) and stencil(8bits) buffer attached
// multisampled: does anti-aliasing
void create_framebuffer_multisampled(u32* tex_buffer, u32* fbo, u32* rbo);
// create a hdr multisampled RGB16F framebuffer with depth(24bits) and stencil(8bits) buffer attached
// multisampled: does anti-aliasing
// RGB16F: 16bit float per red, green and blue channel
void create_framebuffer_multisampled_hdr(u32* tex_buffer, u32* fbo, u32* rbo);
// create a purely depth buffer framebuffer, f.e. used for shadow-mapping
void create_framebuffer_shadowmap(u32* tex_buffer, u32* fbo, int width, int height);
// create a R16F framebuffer, only has a red channel and uses 16bit float instead 8bit
void create_framebuffer_single_channel_f(u32* tex_buffer, u32* fbo, u32* rbo, f32 size_divisor);

// set framebuffer to be the one used for future actions
void bind_framebuffer(u32 fbo);
// deactivate framebuffer
void unbind_framebuffer();

// changes the framebuffers size to the one of the window
void resize_frame_buffer_to_window(u32 tex_buffer, int size_divisor);
// write a multisampled framebuffer(fbo_msaa) into a regular buffer(fbo)
// you need to do this and use the regular buffer (fbo) for displaying to screen
void blit_multisampled_framebuffer(u32 fbo_msaa, u32 fbo);

#endif