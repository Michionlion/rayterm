#ifndef _RAYTERM_RENDERER_H_
#define _RAYTERM_RENDERER_H_

#include <optix.h>
#include <optix_world.h>
#include "raytrace.h"

class Renderer {
    int width, height;

    optix::Context context;
    optix::Buffer pixel_buffer;

    void initOptiX();
    void initWorld();

   public:
    Renderer(int width, int height) : width(width), height(height) {
        initOptiX();
        initWorld();
    }
    ~Renderer() {
        // destroy context
        try {
            context->destroy();
        } catch (const optix::Exception& ex) {
            printf("~Renderer Exception: %d (%s)", ex.getErrorCode(), ex.getErrorString().c_str());
        }
    }

    void resize(int width, int height);

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
