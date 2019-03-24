#include <gtest/gtest.h>
#include <chrono>
#include <cstdio>
#include "raytrace.h"
#include "renderer.h"

#ifndef RES_MULT
#define RES_MULT 1.0
#endif

#ifndef SAMPLES
#define SAMPLES 8
#endif

TEST(RaytracerTest, Screenshot) {
    printDeviceInfo();

    std::chrono::steady_clock::time_point begin, end;
    auto renderer = new Renderer(int(80 * RES_MULT), int(52 * RES_MULT), SAMPLES);

    begin = std::chrono::steady_clock::now();
    renderer->launch();
    end = std::chrono::steady_clock::now();

    printf("Rendered in %ldms\n",
        std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count());

    PixelBuffer* buffer = renderer->buffer()->map();
    begin               = std::chrono::steady_clock::now();
    write_buffer("test_image.ppm", buffer);
    end = std::chrono::steady_clock::now();
    delete buffer;

    printf("Written in %ldms\n",
        std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count());
    delete renderer;
}
