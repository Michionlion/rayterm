#ifndef _RAYTERM_TRANSLATOR_H_
#define _RAYTERM_TRANSLATOR_H_

#include "terminal.h"

POINT* window_to_pixel(float x, float y);
void render(TERMINAL* terminal);

#endif
