#include "unicode_translator.h"
#include <cstdlib>
#include "unicode_buffer.h"

// U+2580 (▀)
#define UPPER_HALF_PIXEL_CHAR 0x2580

UnicodeBuffer* translate_halfpixel(PixelBuffer* pixels) {
    return translate_halfpixel(pixels, new UnicodeBuffer(pixels->width, pixels->height / 2));
}

UnicodeBuffer* translate_halfpixel(PixelBuffer* pixels, UnicodeBuffer* unicode) {
    int cell = 0;
    for (int y = 0; y < pixels->height; y += 2) {
        for (int x = 0; x < pixels->width; x++) {
            auto fg = pixels->get(x, y);
            auto bg = pixels->get(x, y + 1);
            unicode->data[cell++] =
                unicode_cell(UPPER_HALF_PIXEL_CHAR, fg.x, fg.y, fg.z, bg.x, bg.y, bg.z);
        }
    }

    return unicode;
}
