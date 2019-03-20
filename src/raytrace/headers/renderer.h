#ifndef _RAYTERM_RENDERER_H_
#define _RAYTERM_RENDERER_H_

#include <optix.h>
#include <optix_world.h>
#include <map>
#include <string>
#include "camera.h"

// When using a PixelBuffer, you MUST call `delete buffer;` after you've finished.
// This will unmap the memory, which is required before another launch.
class PixelBuffer {
    optix::Buffer backing_buffer;

   public:
    optix::uchar4* data;
    RTsize width, height;

    explicit PixelBuffer(optix::Buffer buffer) {
        backing_buffer = buffer;
        buffer->getSize(width, height);
        data = static_cast<optix::uchar4*>(backing_buffer->map(0, RT_BUFFER_MAP_READ));
    }

    ~PixelBuffer() {
        try {
            backing_buffer->unmap();
        } catch (const optix::Exception& ex) {
            printf("~PixelBuffer Error: %d (%s)\n", ex.getErrorCode(), ex.getErrorString().c_str());
        }
    }

    const optix::uchar4& get(int x, int y) const { return data[y * width + x]; }
};

class Renderer {
    int width, height;

    optix::Context context;
    optix::Buffer pixel_buffer;

    std::map<std::string, optix::Program> programs;

    void initContext();
    void initPrograms();
    void initOptiX();
    void initWorld();

   public:
    Camera* camera;

    Renderer(int width, int height) : width(width), height(height) {
        std::string place = "initContext";
        try {
            initContext();
            place = "initPrograms";
            initPrograms();
            place = "initOptiX";
            initOptiX();
            place = "initWorld";
            initWorld();
        } catch (const optix::Exception& ex) {
            printf("Renderer Error in %s: %d (%s)\n", place.c_str(), ex.getErrorCode(),
                ex.getErrorString().c_str());
            throw std::runtime_error("Renderer Error");
        }
    }
    ~Renderer() {
        // destroy context
        try {
            delete camera;
            context->destroy();
        } catch (const optix::Exception& ex) {
            printf("~Renderer Error: %d (%s)\n", ex.getErrorCode(), ex.getErrorString().c_str());
        }
    }

    void resize(int width, int height);

    PixelBuffer* buffer() { return new PixelBuffer(pixel_buffer); }

    // launch a ray-tracing render
    void launch();
};

// For rtDevice*() function error checking. No OptiX context present at that time.
#define RT_CHECK_ERROR(func)                                                               \
    do {                                                                                   \
        RTresult code = func;                                                              \
        if (code != RT_SUCCESS) {                                                          \
            const char* msg;                                                               \
            rtContextGetErrorString(nullptr, code, &msg);                                  \
            std::cerr << "ERROR " << msg << " (" << code << ") in " << #func << std::endl; \
        }                                                                                  \
    } while (0)

static void printDeviceInfo() {
    unsigned int device_count, i, multiprocessor_count, threads_per_block, clock_rate,
        texture_count, timeout_enabled, tcc_driver, cuda_device;
    unsigned int compute_capability[2];
    char device_name[128];
    RTsize memory_size;

    RTresult code = rtDeviceGetDeviceCount(&device_count);
    if (device_count <= 0 || code != RT_SUCCESS) {
        const char* msg;
        rtContextGetErrorString(nullptr, code, &msg);
        fprintf(stderr, "A supported NVIDIA GPU could not be found\nError: %s (%i)\n", msg, code);
        throw std::runtime_error("No GPU found");
    } else {
        printf("Found %i devices:\n", device_count);
    }

    for (i = 0; i < device_count; i++) {
        RT_CHECK_ERROR(
            rtDeviceGetAttribute(i, RT_DEVICE_ATTRIBUTE_NAME, sizeof(device_name), &device_name));
        RT_CHECK_ERROR(rtDeviceGetAttribute(i, RT_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT,
            sizeof(multiprocessor_count), &multiprocessor_count));
        RT_CHECK_ERROR(rtDeviceGetAttribute(i, RT_DEVICE_ATTRIBUTE_MAX_THREADS_PER_BLOCK,
            sizeof(threads_per_block), &threads_per_block));
        RT_CHECK_ERROR(rtDeviceGetAttribute(
            i, RT_DEVICE_ATTRIBUTE_CLOCK_RATE, sizeof(clock_rate), &clock_rate));
        RT_CHECK_ERROR(rtDeviceGetAttribute(
            i, RT_DEVICE_ATTRIBUTE_TOTAL_MEMORY, sizeof(memory_size), &memory_size));
        RT_CHECK_ERROR(rtDeviceGetAttribute(i, RT_DEVICE_ATTRIBUTE_MAX_HARDWARE_TEXTURE_COUNT,
            sizeof(texture_count), &texture_count));
        RT_CHECK_ERROR(rtDeviceGetAttribute(i, RT_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY,
            sizeof(compute_capability), &compute_capability));
        RT_CHECK_ERROR(rtDeviceGetAttribute(i, RT_DEVICE_ATTRIBUTE_EXECUTION_TIMEOUT_ENABLED,
            sizeof(timeout_enabled), &timeout_enabled));
        RT_CHECK_ERROR(rtDeviceGetAttribute(
            i, RT_DEVICE_ATTRIBUTE_TCC_DRIVER, sizeof(tcc_driver), &tcc_driver));
        RT_CHECK_ERROR(rtDeviceGetAttribute(
            i, RT_DEVICE_ATTRIBUTE_CUDA_DEVICE_ORDINAL, sizeof(cuda_device), &cuda_device));
        printf(
            "  Device %u: %s with:\n    %u multiprocessors\n    %u threads per block\n    "
            "%u kHz\n    %lu bytes global memory\n    %u hardware textures\n    "
            "compute capability %u.%u\n    timeout %sabled\n    "
            "Tesla compute cluster driver %sabled\n    cuda device %u\n",
            i, device_name, multiprocessor_count, threads_per_block, clock_rate, memory_size,
            texture_count, compute_capability[0], compute_capability[1],
            timeout_enabled ? "en" : "dis", tcc_driver ? "en" : "dis", cuda_device);
    }
}

#endif
