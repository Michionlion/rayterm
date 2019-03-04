#ifndef _RAYTERM_RENDERER_H_
#define _RAYTERM_RENDERER_H_

#include <optix.h>
#include <optix_world.h>
#include <map>
#include <string>

// When using a PixelBuffer, you MUST call `delete buffer;` after you've finished.
// This will unmap the memory, which is required before another launch.
class PixelBuffer {
    optix::Buffer backing_buffer;

   public:
    optix::float4* data;
    RTsize width, height;

    explicit PixelBuffer(optix::Buffer buffer) {
        backing_buffer = buffer;
        buffer->getSize(width, height);
        data = static_cast<optix::float4*>(backing_buffer->map(0, RT_BUFFER_MAP_READ));
    }

    ~PixelBuffer() {
        try {
            backing_buffer->unmap();
        } catch (const optix::Exception& ex) {
            printf("~PixelBuffer Error: %d (%s)\n", ex.getErrorCode(), ex.getErrorString().c_str());
        }
    }

    const optix::float4& get(int x, int y) const { return data[y * width + x]; }
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
    Renderer(int width, int height) : width(width), height(height) {
        try {
            initContext();
            initPrograms();
            initOptiX();
            initWorld();
        } catch (const optix::Exception& ex) {
            printf("Renderer Error: %d (%s)\n", ex.getErrorCode(), ex.getErrorString().c_str());
            exit(1);
        }
    }
    ~Renderer() {
        // destroy context
        try {
            context->destroy();
        } catch (const optix::Exception& ex) {
            printf("~Renderer Error: %d (%s)\n", ex.getErrorCode(), ex.getErrorString().c_str());
        }
    }

    void resize(int width, int height);

    PixelBuffer* buffer() { return new PixelBuffer(pixel_buffer); }

    // launch a ray-tracing render
    void launch();

    void printSystemInfo();
};

// For rtDevice*() function error checking. No OptiX context present at that time.
#define RT_CHECK_ERROR_NO_CONTEXT(func)                            \
    do {                                                           \
        RTresult code = func;                                      \
        if (code != RT_SUCCESS)                                    \
            std::cerr << "ERROR: Function " << #func << std::endl; \
    } while (0)

#endif
