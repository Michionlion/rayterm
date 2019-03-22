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

    outfile << "P6 ";
    outfile << buffer->width << " " << buffer->height << " " << 255 << "\n";
    int pixels = buffer->width * buffer->height;
    int pixel  = 0;
    for (int y = 0; y < buffer->height; y++) {
        for (int x = 0; x < buffer->width; x++) {
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
    delete buffer;
    return 0;
}
