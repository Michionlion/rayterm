#include <chrono>
#include <cstdio>
#include <cstring>
#include "rayterm"
#include "raytrace.h"
#include "renderer.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: rtexplore <N> <RES_MULT>");
        exit(1);
    }

    int n        = std::stoi(argv[1]);
    int res_mult = std::stof(argv[2]);
    int width    = int(80 * res_mult);
    int height   = int(52 * res_mult);
    printDeviceInfo();
    printf("Rendering %dx%d image\n", width, height);

    auto renderer = new Renderer(width, height);
    std::chrono::steady_clock::time_point begin, end;
    long times[n];

    for (int i = 0; i < n; i++) {
        delete renderer;
        renderer = new Renderer(width, height);

        printf("Start run %d\n", i + 1);
        begin = std::chrono::steady_clock::now();
        renderer->launch();
        end = std::chrono::steady_clock::now();
        printf("End run %d, ", i + 1);

        times[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
        printf("time: %ldms\n", times[i]);
    }

    long average_time = 0;
    for (int i = 0; i < n; i++) {
        average_time += times[i];
    }
    average_time /= n;

    printf("\nSUMMARY\n\nRendered on average in %ldms\n", average_time);

    begin = std::chrono::steady_clock::now();
    write_buffer("test_image.ppm", renderer->buffer());
    end = std::chrono::steady_clock::now();

    printf("\nWritten in %ldms\n\n-------\n",
        std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count());
}
