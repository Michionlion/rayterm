#include "raytrace.h"
#include <cstdio>

#ifndef SAMPLES
#define SAMPLES 32
#endif

#ifndef RES_MULT
#define RES_MULT 1.0
#endif

int launch(progress_callback cb /* = nullptr */) {
    FILE* outfile = fopen("test_image.ppm", "w");
    if (outfile == nullptr) {
        fprintf(stderr, "Error opening %s!\n", "test_image.ppm");
        return 1;
    }

    int width  = int(80 * RES_MULT);
    int height = int(52 * RES_MULT);

    fprintf(outfile, "P3\n%i %i\n255\n", width, height);
    float pixels = width * height;
    int pixel    = 0;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            for (int sample = 0; sample < SAMPLES; sample++) {
            }

            // gamma correct outcol
            // raise to 1 / gamma (1/2 in our case)

            fprintf(outfile, "%i %i %i\n", 0, 0, 0);
            if (cb != nullptr) {
                cb(pixel++ / pixels);
            }
        }
    }
    fclose(outfile);
    return 0;
}
