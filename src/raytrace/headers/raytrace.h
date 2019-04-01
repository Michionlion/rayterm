#ifndef _RAYTERM_RAYTRACER_H_
#define _RAYTERM_RAYTRACER_H_

#include <optix.h>
#include <optix_world.h>
#include "renderer.h"

// parameters are (complete, total)
using progress_callback = void (*)(unsigned int, unsigned int);

int write_buffer(const char* filename, PixelBuffer* buffer, progress_callback cb = nullptr);

#endif
