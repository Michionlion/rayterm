#include "raytrace.h"
#include <optix.h>
#include <optix_world.h>
#include <cstdio>
#include <fstream>

int write_buffer(const char* filename, PixelBuffer* buffer, progress_callback cb /* = nullptr */) {
    std::fstream outfile(filename, std::fstream::out | std::fstream::binary | std::fstream::trunc);
    if (outfile.fail()) {
        fprintf(stderr, "Error opening %s!\n", filename);
        return 1;
    }

    buffer->map();

    auto width  = static_cast<unsigned int>(buffer->width);
    auto height = static_cast<unsigned int>(buffer->height);

    outfile << "P6 ";
    outfile << width << " " << height << " " << 255 << "\n";
    unsigned int pixels = width * height;
    unsigned int pixel  = 0;
    for (unsigned int y = 0; y < height; y++) {
        for (unsigned int x = 0; x < width; x++) {
            optix::uchar4 sample = buffer->get(x, y);

            // print red
            outfile << sample.x;
            // print green
            outfile << sample.y;
            // print blue
            outfile << sample.z;
            if (cb != nullptr) {
                cb(pixel++, pixels);
            }
        }
    }
    outfile.close();
    return 0;
}
