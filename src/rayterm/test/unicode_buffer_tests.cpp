#include <gtest/gtest.h>
#include <optix.h>
#include <optix_world.h>
#include "raytrace"
#include "unicode_buffer.h"
#include "unicode_translator.h"

TEST(UnicodeBuffer, HalfPixelTranslation) {
    // create renderer so we have access to context
    int width                  = 4;
    int height                 = 8;  // must be even
    auto renderer              = new Renderer(width, height, 1);
    optix::Buffer pixel_buffer = renderer->getContext()->createBuffer(RT_BUFFER_OUTPUT);
    pixel_buffer->setFormat(RT_FORMAT_UNSIGNED_BYTE4);
    pixel_buffer->setSize(width, height);
    auto pixel_data = static_cast<optix::uchar4*>(pixel_buffer->map());
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            pixel_data[y * width + x] = optix::make_uchar4(x, y, x + y, 255);
        }
    }
    pixel_buffer->unmap();
    PixelBuffer* pixels = new PixelBuffer(pixel_buffer);

    // for (int y = 0; y < height; y++) {
    //     for (int x = 0; x < width; x++) {
    //         optix::uchar4 pix = pixels->get(x, y);
    //         printf("(%d,%d,%d)", pix.x, pix.y, pix.z);
    //     }
    //     printf("\n");
    // }

    UnicodeBuffer* unicode = translate_halfpixel(pixels);

    // printf("\n\n");
    // for (int y = 0; y < height / 2; y++) {
    //     for (int x = 0; x < width; x++) {
    //         unicode_cell cell = unicode->get(x, y);
    //         printf("(%d,%d,%d:%d,%d,%d)", cell.fg_r, cell.fg_g, cell.fg_b, cell.bg_r, cell.bg_g,
    //             cell.bg_b);
    //     }
    //     printf("\n");
    // }

    for (int y = 0; y < height / 2; y++) {
        for (int x = 0; x < width; x++) {
            unicode_cell cell = unicode->get(x, y);
            EXPECT_EQ(int(cell.fg_r), x);
            EXPECT_EQ(int(cell.fg_g), y * 2);
            EXPECT_EQ(int(cell.fg_b), x + y * 2);
            EXPECT_EQ(int(cell.bg_r), x);
            EXPECT_EQ(int(cell.bg_g), y * 2 + 1);
            EXPECT_EQ(int(cell.bg_b), x + y * 2 + 1);
        }
    }
}
