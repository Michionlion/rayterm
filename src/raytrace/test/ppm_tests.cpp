#include <gtest/gtest.h>
#include <chrono>
#include <cstdio>
#include "raytrace.h"
#include "renderer.h"

#ifndef RES_MULT
#define RES_MULT 1.0
#endif

void progress_cb(int current, int total) {
#ifndef NO_PROGRESS
    printf("\b\b\b%02d%%", int(current * 100.0 / total));
#endif
}

TEST(RaytracerTest, Success) {
    auto renderer = new Renderer(int(80 * RES_MULT), int(52 * RES_MULT));

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    renderer->launch();

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    printf("Rendered in %ldms\n",
        std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count());

    printf("Writing Image...\n00%%");
    write_buffer("test_image.ppm", renderer->buffer(), progress_cb);
    printf("\b\b\b100%% -- Render Complete!\n");
}
