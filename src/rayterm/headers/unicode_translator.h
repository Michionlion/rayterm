#ifndef _RAYTERM_UNICODE_TRANSLATOR_H_
#define _RAYTERM_UNICODE_TRANSLATOR_H_

#include "raytrace"
#include "unicode_buffer.h"

UnicodeBuffer* translate_halfpixel(PixelBuffer* pixels);
UnicodeBuffer* translate_halfpixel(PixelBuffer* pixels, UnicodeBuffer* unicode);

#endif
