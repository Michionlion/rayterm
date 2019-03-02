#include <gtest/gtest.h>
#include <chrono>
#include <cstdio>
#include "renderer.h"

#ifndef RES_MULT
#define RES_MULT 1.0
#endif

void progress_cb(float progress) {
#ifndef NO_PROGRESS
    printf("\b\b\b%02d%%", int(progress * 100));
#endif
}

TEST(RaytracerTest, Success) {
    auto renderer = new Renderer(int(80 * RES_MULT), int(52 * RES_MULT));

    // printf("00%%");
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    renderer->launch();

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    // printf("\b\b\b100%% -- Render Complete!\n");

    printf("Completed in %ldms\n",
        std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count());
}
