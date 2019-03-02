#ifndef _RAYTERM_RAYTRACER_H_
#define _RAYTERM_RAYTRACER_H_

using progress_callback = void (*)(float);

int launch(progress_callback cb = nullptr);

#endif
