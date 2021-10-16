#pragma once
#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "global.h"
#include "GLAD/glad.h"

typedef enum framebuffer_type
{
	FRAMEBUFFER_RGB				 = GL_RGB,
	FRAMEBUFFER_RGB16F			 = GL_RGBA16F, // check if GL_RGB16F works as well
	FRAMEBUFFER_SINGLE_CHANNEL	 = GL_RED,
	FRAMEBUFFER_SINGLE_CHANNEL_F = GL_R16F,
	FRAMEBUFFER_DEPTH			 = GL_DEPTH_COMPONENT,
	
}framebuffer_type;
typedef struct framebuffer
{
	u32 buffer;
	u32 fbo;
	u32 rbo;

	framebuffer_type type;

	// use either not both
	int size_divisor;
	int width, height;

	bee_bool is_msaa;
	int samples;

}framebuffer;


// create a basic RGB framebuffer with depth(24bits) and stencil(8bits) buffer attached
void create_framebuffer_rgb(u32* tex_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height);
// create a hdr RGB16F framebuffer with depth(24bits) and stencil(8bits) buffer attached
// RGB16F: 16bit float per red, green and blue channel
void create_framebuffer_hdr(u32* tex_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height);
// create a multisampled RGB framebuffer with depth(24bits) and stencil(8bits) buffer attached
// multisampled: does anti-aliasing
void create_framebuffer_multisampled(u32* tex_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height, int samples);
// create a hdr multisampled RGB16F framebuffer with depth(24bits) and stencil(8bits) buffer attached
// multisampled: does anti-aliasing
// RGB16F: 16bit float per red, green and blue channel
void create_framebuffer_multisampled_hdr(u32* tex_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height, int samples);
// create a purely depth buffer framebuffer, f.e. used for shadow-mapping
void create_framebuffer_shadowmap(u32* tex_buffer, u32* fbo, int width, int height);
// create a R16F framebuffer, only has a red channel and uses 16bit float instead 8bit unsigned integer
void create_framebuffer_single_channel_f(u32* tex_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height);
// create a RED framebuffer, only has a red channel and uses 8bit unsigned integer
void create_framebuffer_single_channel(u32* tex_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height);

// creates a framebuffer as specified in "fb"
// when "width" & "height" are 0, they will be set automatically
// when "size_divisor" is 0 it will be set to 1, doing nothing
// when "samples" is 0 or less it will be set to 4
// !!! setting "width" & "height" only affects "FRAMEBUFFER_DEPTH" 
// !!! only "FRAMEBUFFER_RGB" and "FRAMEBUFFER_RGB16F" can be set to use multisampling via"is_msaa"
rtn_code create_framebuffer(framebuffer* fb);

// set framebuffer to be the one used for future actions
void bind_framebuffer(framebuffer* fb);
// set framebuffer to be the one used for future actions
void bind_framebuffer_fbo(u32 fbo);
// deactivate framebuffer
void unbind_framebuffer();

// changes the framebuffers size to the one of the window
void resize_frame_buffer_to_window(framebuffer* fb);
// write a multisampled framebuffer(fb_msaa) into a regular buffer(fb)
// you need to do this and use the regular buffer (fb) for displaying to screen
void blit_multisampled_framebuffer(framebuffer* fb_msaa, framebuffer* fb);
// write a multisampled framebuffer(fbo_msaa) into a regular buffer(fbo)
// you need to do this and use the regular buffer (fbo) for displaying to screen
void blit_multisampled_framebuffer_fbo(u32 fbo_msaa, u32 fbo);

#endif