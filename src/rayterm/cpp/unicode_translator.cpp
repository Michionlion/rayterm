#include "unicode_translator.h"
#include <cstdlib>
#include "unicode_buffer.h"

// U+2580 (▀)
#define UPPER_HALF_PIXEL_CHAR 0x2580

UnicodeBuffer* translate_halfpixel(PixelBuffer* pixels) {
    int num_pixels    = pixels->width * pixels->height;
    int num_cells     = num_pixels / 2;
    auto unicode_data = static_cast<unicode_cell*>(malloc(num_cells * sizeof(unicode_cell)));

    int cell = 0;
    for (int y = 0; y < pixels->height; y += 2) {
        for (int x = 0; x < pixels->width; x++) {
            auto fg = pixels->get(x, y);
            auto bg = pixels->get(x, y + 1);
            unicode_data[cell++] =
                unicode_cell(UPPER_HALF_PIXEL_CHAR, fg.x, fg.y, fg.z, bg.x, bg.y, bg.z);
        }
    }

    return new UnicodeBuffer(unicode_data, pixels->width, pixels->height / 2);
}
