#include "unicode_translator.h"
#include <cstdlib>
#include "unicode_buffer.h"

UnicodeBuffer* translate_halfpixel(PixelBuffer* pixels) {
    return translate_halfpixel(pixels, new UnicodeBuffer(pixels->width, pixels->height / 2));
}

UnicodeBuffer* translate_halfpixel(PixelBuffer* pixels, UnicodeBuffer* unicode) {
    int cell            = 0;
    unsigned int height = static_cast<unsigned int>(pixels->height);
    unsigned int width  = static_cast<unsigned int>(pixels->width);
    for (unsigned int y = 0; y < height; y += 2) {
        for (unsigned int x = 0; x < width; x++) {
            auto fg               = pixels->get(x, y);
            auto bg               = pixels->get(x, y + 1);
            unicode->data[cell++] = unicode_cell(fg.x, fg.y, fg.z, bg.x, bg.y, bg.z);
        }
    }

    return unicode;
}
