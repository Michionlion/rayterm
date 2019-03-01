#ifndef _RAYTERM_RAYTRACER_H_
#define _RAYTERM_RAYTRACER_H_
#include <cstddef>

typedef void (*progress_callback)(float);

int launch(progress_callback cb = NULL);

#endif
