#include <chrono>
#include <cstdio>
#include <cstring>
#include <exception>
#include "rayterm"
#include "raytrace.h"
#include "renderer.h"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: rtime <N> <RES_MULT> <SAMPLES>\n");
        exit(1);
    }
    try {
        int n          = std::stoi(argv[1]);
        float res_mult = std::stof(argv[2]);
        int samples    = std::stoi(argv[3]);
        int width      = int(80 * res_mult);
        int height     = int(52 * res_mult);
        printDeviceInfo();
        printf("Rendering %dx%d, %dspp images\n", width, height, samples);

        auto renderer = new Renderer(width, height, samples);
        std::chrono::steady_clock::time_point begin, end;
        int64_t times[n + 1];

        for (int i = 0; i < n + 1; i++) {
            // delete renderer;
            // renderer = new Renderer(width, height, samples);

            if (i > 0)
                printf("Start run %d\n", i);
            begin = std::chrono::steady_clock::now();
            renderer->launch();
            end = std::chrono::steady_clock::now();
            if (i > 0)
                printf("End run %d, ", i);

            times[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
            if (i > 0)
                printf("time: %ldms\n", times[i]);
        }

        int64_t average_time = 0;
        for (int i = 1; i < n + 1; i++) {
            average_time += times[i];
        }
        average_time /= n;

        printf("\nSUMMARY\n\nRendered %dx%d, %dspp images in %ldms on average\n", width, height,
            samples, average_time);
        printf("First run (loading) took %ldms\n", times[0]);

        begin = std::chrono::steady_clock::now();
        write_buffer("test_image.ppm", renderer->buffer());
        end = std::chrono::steady_clock::now();

        printf("\nWritten in %ldms\n\n-------\n",
            std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count());
    } catch (const std::exception& ex) {
        printf("Exception in rtime: (%s)\n", ex.what());
        exit(1);
    }
}
