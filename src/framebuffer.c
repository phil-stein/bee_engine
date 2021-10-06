#include "framebuffer.h"

#include "GLAD/glad.h"

#include "window.h"


// ---- vars ----
u32 fbo_ms; // multisample fbo
u32 rbo_ms; // multisample rbo
u32 fbo;    // color texture fbo
u32 rbo;	// color texture rbo


void create_framebuffer_multisampled(u32* tex_buffer)
{
	// create framebuffer object
	glGenFramebuffers(1, &fbo_ms);
	// set fbo to be the active framebuffer to be modified
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_ms);

	// generate texture
	glGenTextures(1, tex_buffer);
	// glBindTexture(GL_TEXTURE_2D, *tex_buffer);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, *tex_buffer);

	int w, h; get_window_size(&w, &h);
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, w, h, GL_TRUE); // 4 samples
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// glBindTexture(GL_TEXTURE_2D, 0);
	// attach it to currently bound framebuffer object
	// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *tex_buffer, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, *tex_buffer, 0);

	// create render buffer object
	glGenRenderbuffers(1, &rbo_ms);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo_ms);
	// glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, w, h);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach render buffer object to the depth and stencil buffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo_ms);

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

void create_framebuffer(u32* tex_buffer)
{
	// create framebuffer object
	glGenFramebuffers(1, &fbo);
	// set fbo to be the active framebuffer to be modified
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	int w, h; get_window_size(&w, &h);
	// generate texture
	glGenTextures(1, tex_buffer);
	glBindTexture(GL_TEXTURE_2D, *tex_buffer);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// glBindTexture(GL_TEXTURE_2D, 0);
	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *tex_buffer, 0);

	// create render buffer object
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
	// glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, w, h);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach render buffer object to the depth and stencil buffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

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

void bind_framebuffer(bee_bool multisampled)
{
	if (multisampled)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fbo_ms);
	}
	else
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	}
}
void unbind_framebuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void resize_frame_buffer_to_window(u32* tex_buffer)
{
	glBindTexture(GL_TEXTURE_2D, *tex_buffer);

	int w, h; get_window_size(&w, &h);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
}

void blit_multisampled_framebuffer()
{
	int w, h;
	get_window_size(&w, &h);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_ms);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
	glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

