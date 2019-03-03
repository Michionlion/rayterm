#include "raytrace.h"
#include <optix.h>
#include <optix_world.h>
#include <cstdio>
#include <fstream>

#ifndef SAMPLES
#define SAMPLES 32
#endif

#ifndef RES_MULT
#define RES_MULT 1.0
#endif

float to_bt_709(float value) {
    if (value < 0.018) {
        return value * 4.5;
    } else {
        return 1.099 * pow(value, 0.45) - 0.099;
    }
}

uint8_t colorize(float value) {
    value = to_bt_709(value);
    if (value >= 1.0) {
        return 255;
    } else if (value <= 0.0) {
        return 0;
    }

    return uint8_t(value * 255);
}

int write_buffer(const char* filename, PixelBuffer* buffer, progress_callback cb /* = nullptr */) {
    std::fstream outfile(filename, std::fstream::out | std::fstream::binary | std::fstream::trunc);
    if (outfile.fail()) {
        fprintf(stderr, "Error opening %s!\n", filename);
        return 1;
    }

    auto width  = int(80 * RES_MULT);
    auto height = int(52 * RES_MULT);

    outfile << "P6 ";
    outfile << width << " " << height << " " << 255 << "\n";
    int pixels = width * height;
    int pixel  = 0;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // gamma correct outcol
            // raise to 1 / gamma (1/2 in our case)

            optix::float4 sample = buffer->get(x, y);

            // print red
            outfile << colorize(sample.x);
            // print green
            outfile << colorize(sample.y);
            // print blue
            outfile << colorize(sample.z);
            if (cb != nullptr) {
                cb(pixel++, pixels);
            }
        }
    }
    outfile.close();
    delete buffer;
    return 0;
}
