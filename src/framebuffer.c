#include "framebuffer.h"

#include "GLAD/glad.h"

#include "window.h"


void create_framebuffer_rgb(u32* tex_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height)
{
	// create framebuffer object
	glGenFramebuffers(1, fbo);
	// set fbo to be the active framebuffer to be modified
	glBindFramebuffer(GL_FRAMEBUFFER, *fbo);

	int w, h; get_window_size(&w, &h);
	// scale the resolution 
	w /= size_divisor;
	h /= size_divisor;
	*width  = w;
	*height = h;

	// generate texture
	glGenTextures(1, tex_buffer);
	glBindTexture(GL_TEXTURE_2D, *tex_buffer);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// glBindTexture(GL_TEXTURE_2D, 0);
	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *tex_buffer, 0);

	// create render buffer object
	glGenRenderbuffers(1, rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, &rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
	// glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, w, h);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach render buffer object to the depth and stencil buffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, &rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		fprintf(stderr, "-!!!-> ERROR_CREATING_FRAMEBUFFER");
		assert(0);
	}

	// unbind the framebuffer, opengl now renders to the default buffer again
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// free memory
	// glDeleteFramebuffers(1, &fbo);
}

void create_framebuffer_hdr(u32* tex_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height)
{
	// create framebuffer object
	glGenFramebuffers(1, fbo);
	// set fbo to be the active framebuffer to be modified
	glBindFramebuffer(GL_FRAMEBUFFER, *fbo);

	int w, h; get_window_size(&w, &h);
	// scale the resolution 
	w /= size_divisor;
	h /= size_divisor;
	*width = w;
	*height = h;

	// generate texture
	glGenTextures(1, tex_buffer);
	glBindTexture(GL_TEXTURE_2D, *tex_buffer);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// glBindTexture(GL_TEXTURE_2D, 0);
	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *tex_buffer, 0);

	// create render buffer object
	glGenRenderbuffers(1, rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, &rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
	// glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, w, h);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach render buffer object to the depth and stencil buffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, &rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		fprintf(stderr, "-!!!-> ERROR_CREATING_FRAMEBUFFER");
		assert(0);
	}

	// unbind the framebuffer, opengl now renders to the default buffer again
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// free memory
	// glDeleteFramebuffers(1, &fbo);
}

void create_framebuffer_multisampled(u32* tex_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height, int samples)
{
	// create framebuffer object
	glGenFramebuffers(1, fbo);
	// set fbo to be the active framebuffer to be modified
	glBindFramebuffer(GL_FRAMEBUFFER, *fbo);

	// generate texture
	glGenTextures(1, tex_buffer);
	// glBindTexture(GL_TEXTURE_2D, *tex_buffer);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, *tex_buffer);

	int w, h; get_window_size(&w, &h);
	// scale the resolution 
	w /= size_divisor;
	h /= size_divisor;
	*width = w;
	*height = h;

	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, w, h, GL_TRUE); // 4 samples
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// glBindTexture(GL_TEXTURE_2D, 0);
	// attach it to currently bound framebuffer object
	// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *tex_buffer, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, *tex_buffer, 0);

	// create render buffer object
	glGenRenderbuffers(1, rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, *rbo);
	// glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, w, h);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach render buffer object to the depth and stencil buffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, *rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		fprintf(stderr, "-!!!-> ERROR_CREATING_FRAMEBUFFER");
		assert(0);
	}

	// unbind the framebuffer, opengl now renders to the default buffer again
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// free memory
	// glDeleteFramebuffers(1, &fbo);
}

void create_framebuffer_multisampled_hdr(u32* tex_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height, int samples)
{
	// create framebuffer object
	glGenFramebuffers(1, fbo);
	// set fbo to be the active framebuffer to be modified
	glBindFramebuffer(GL_FRAMEBUFFER, *fbo);

	// generate texture
	glGenTextures(1, tex_buffer);
	// glBindTexture(GL_TEXTURE_2D, *tex_buffer);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, *tex_buffer);

	int w, h; get_window_size(&w, &h);
	// scale the resolution 
	w /= size_divisor;
	h /= size_divisor;
	*width = w;
	*height = h;

	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB16F, w, h, GL_TRUE); // 4 samples
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, *tex_buffer, 0);

	// create render buffer object
	glGenRenderbuffers(1, rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, *rbo);
	// glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, w, h);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach render buffer object to the depth and stencil buffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, *rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		fprintf(stderr, "-!!!-> ERROR_CREATING_FRAMEBUFFER");
		assert(0);
	}

	// unbind the framebuffer, opengl now renders to the default buffer again
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// free memory
	// glDeleteFramebuffers(1, &fbo);
}

void create_framebuffer_shadowmap(u32* tex_buffer, u32* fbo, int width, int height)
{
	// create framebuffer object
	glGenFramebuffers(1, fbo);
	// set fbo to be the active framebuffer to be modified
	bind_framebuffer_fbo(*fbo);

	// gen texture
	glGenTextures(1, tex_buffer);
	glBindTexture(GL_TEXTURE_2D, *tex_buffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	f32 border_col[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_col);

	bind_framebuffer_fbo(*fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, *tex_buffer, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	// unbind the framebuffer, opengl now renders to the default buffer again
	unbind_framebuffer();
}

void create_framebuffer_single_channel(u32* tex_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height)
{
	// create framebuffer object
	glGenFramebuffers(1, fbo);
	// set fbo to be the active framebuffer to be modified
	glBindFramebuffer(GL_FRAMEBUFFER, *fbo);

	int w, h; get_window_size(&w, &h);
	// scale the resolution 
	w /= size_divisor;
	h /= size_divisor;
	*width = w;
	*height = h;

	// generate texture
	glGenTextures(1, tex_buffer);
	glBindTexture(GL_TEXTURE_2D, *tex_buffer);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, NULL); // GL_UNSIGNED_BYTE
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// glBindTexture(GL_TEXTURE_2D, 0);
	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *tex_buffer, 0);

	// create render buffer object
	glGenRenderbuffers(1, rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, &rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
	// glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, w, h);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach render buffer object to the depth and stencil buffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, &rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		fprintf(stderr, "-!!!-> ERROR_CREATING_FRAMEBUFFER");
		assert(0);
	}

	// unbind the framebuffer, opengl now renders to the default buffer again
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// free memory
	// glDeleteFramebuffers(1, &fbo);
}

void create_framebuffer_single_channel_f(u32* tex_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height)
{
	// create framebuffer object
	glGenFramebuffers(1, fbo);
	// set fbo to be the active framebuffer to be modified
	glBindFramebuffer(GL_FRAMEBUFFER, *fbo);

	int w, h; get_window_size(&w, &h);
	// scale the resolution 
	w /= size_divisor;
	h /= size_divisor;
	*width = w;
	*height = h;

	// generate texture
	glGenTextures(1, tex_buffer);
	glBindTexture(GL_TEXTURE_2D, *tex_buffer);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, w, h, 0, GL_RED, GL_FLOAT, NULL); // GL_UNSIGNED_BYTE
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// glBindTexture(GL_TEXTURE_2D, 0);
	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *tex_buffer, 0);

	// create render buffer object
	glGenRenderbuffers(1, rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, &rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
	// glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, w, h);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach render buffer object to the depth and stencil buffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, &rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		fprintf(stderr, "-!!!-> ERROR_CREATING_FRAMEBUFFER");
		assert(0);
	}

	// unbind the framebuffer, opengl now renders to the default buffer again
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// free memory
	// glDeleteFramebuffers(1, &fbo);
}


rtn_code create_framebuffer(framebuffer* fb)
{
	fb->size_divisor = fb->size_divisor <= 0 ? 1 : fb->size_divisor;

	if (fb->is_msaa) // multisampled
	{
		if (fb->type == FRAMEBUFFER_RGB)
		{
			fb->samples = fb->samples <= 0 ? 4 : fb->samples;
			create_framebuffer_multisampled(&fb->buffer, &fb->fbo, &fb->rbo, fb->size_divisor, &fb->width, &fb->height, fb->samples);
			return BEE_OK;
		}
		else if (fb->type == FRAMEBUFFER_RGB16F)
		{
			fb->samples = fb->samples <= 0 ? 4 : fb->samples;
			create_framebuffer_multisampled_hdr(&fb->buffer, &fb->fbo, &fb->rbo, fb->size_divisor, &fb->width, &fb->height, fb->samples);
			return BEE_OK;
		}
		else
		{
			set_error_popup(GENERAL_ERROR, "Tried to create a MSAA Framebuffer of illegal type.");
			printf("[ERROR] Tried to create a MSAA Framebuffer of illegal type.\n");
		}
	}
	else 
	{
		if (fb->type == FRAMEBUFFER_RGB)
		{
			create_framebuffer_rgb(&fb->buffer, &fb->fbo, &fb->rbo, fb->size_divisor, &fb->width, &fb->height);
			return BEE_OK;
		}
		else if (fb->type == FRAMEBUFFER_RGB16F)
		{
			create_framebuffer_hdr(&fb->buffer, &fb->fbo, &fb->rbo, fb->size_divisor, &fb->width, &fb->height);
			return BEE_OK;
		}
		else if (fb->type == FRAMEBUFFER_SINGLE_CHANNEL)
		{
			create_framebuffer_single_channel(&fb->buffer, &fb->fbo, &fb->rbo, fb->size_divisor, &fb->width, &fb->height);
		}
		else if (fb->type == FRAMEBUFFER_SINGLE_CHANNEL_F)
		{
			create_framebuffer_single_channel_f(&fb->buffer, &fb->fbo, &fb->rbo, fb->size_divisor, &fb->width, &fb->height);
			return BEE_OK;
		}
		else if (fb->type == FRAMEBUFFER_SINGLE_CHANNEL)
		{
			if (fb->width == 0 || fb->height == 0) 
			{
				get_window_size(&fb->width, &fb->height);
			}
			create_framebuffer_shadowmap(&fb->buffer, &fb->fbo, &fb->rbo, fb->width, fb->height);
			return BEE_OK;
		}
		else
		{
			set_error_popup(GENERAL_ERROR, "Tried to create a Framebuffer of unknown type.");
			printf("[ERROR] Tried to create a Framebuffer of unknown type.\n");
		}

	}
	return BEE_ERROR;
}

void bind_framebuffer(framebuffer* fb)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fb->fbo);
}
void bind_framebuffer_fbo(u32 fbo)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}
void unbind_framebuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void resize_frame_buffer_to_window(framebuffer* fb)
{
	glBindTexture(GL_TEXTURE_2D, fb->buffer);

	int w, h; get_window_size(&w, &h);
	if (fb->size_divisor > 0)
	{
		w /= fb->size_divisor;
		h /= fb->size_divisor;
	}
	fb->width  = w;
	fb->height = h;

	switch (fb->type)
	{
		case FRAMEBUFFER_RGB:
			if (fb->is_msaa)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, fb->samples, GL_RGB, w, h, GL_TRUE);
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			}
			break;
		case FRAMEBUFFER_RGB16F:
			if (fb->is_msaa)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, fb->samples, GL_RGB16F, w, h, GL_TRUE);
			}
			else 
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			}
			break;
		case FRAMEBUFFER_SINGLE_CHANNEL:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
			break;
		case FRAMEBUFFER_SINGLE_CHANNEL_F:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, w, h, 0, GL_RED, GL_FLOAT, NULL);
			break;
		case FRAMEBUFFER_DEPTH:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			break;
	}
}

void blit_multisampled_framebuffer(framebuffer* fb_msaa, framebuffer* fb)
{
	int w, h;
	// get_window_size(&w, &h);
	w = fb->width;
	h = fb->height;
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fb_msaa->fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb->fbo);
	glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}
void blit_multisampled_framebuffer_fbo(u32 fbo_msaa, u32 fbo)
{
	int w, h;
	get_window_size(&w, &h);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_msaa);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
	glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

