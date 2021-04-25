#pragma once
#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "global.h"

void create_framebuffer(u32* tex_buffer);

void bind_framebuffer();
void unbind_framebuffer();

#endif