#include "renderer.h"
#include <optix.h>
#include <optix_world.h>
#include <iostream>
#include <string>
#include <vector>
#include "build_variables.h"
#include "programs.h"
#include "resources.h"

#define HANDLE_EXCEPTIONS 1
#define TRACE_DEPTH 7

void Renderer::initContext() {
    uint32_t device_count;
    RTresult code = rtDeviceGetDeviceCount(&device_count);
    if (device_count <= 0 || code != RT_SUCCESS) {
        const char* msg;
        rtContextGetErrorString(nullptr, code, &msg);
        fprintf(stderr, "A supported NVIDIA GPU could not be found\nError: %s (%i)\n", msg, code);
        throw std::runtime_error("No GPU found");
    }
    const int RTX = true;
    if (rtGlobalSetAttribute(RT_GLOBAL_ATTRIBUTE_ENABLE_RTX, sizeof(RTX), &RTX) != RT_SUCCESS) {
        fprintf(stderr, "Error setting RTX mode\n");
    } else {
        printf("OptiX RTX execution mode is %s\n", (RTX) ? "on" : "off");
    }

    context = optix::Context::create();

    // specify context details
    context->setMaxTraceDepth(TRACE_DEPTH);

    std::vector<int> devices;
    devices.push_back(0);

    context->setDevices(devices.begin(), devices.end());

    printf("Using device: %d\n", context->getEnabledDevices()[0]);

#if HANDLE_EXCEPTIONS
    context->setPrintEnabled(true);
    context->setExceptionEnabled(RT_EXCEPTION_ALL, true);
    printf("Enabled Exceptions\n");
#endif
}

void Renderer::initPrograms() {
    programs = new Programs(context);

    // load special programs
    programs->load("pinhole_camera", "raygen", "global_raygen");
    programs->load("exception", "exception", "global_exception");
    programs->load("misses", "miss_gradient", "global_miss");
}

void Renderer::initOptiX() {
    // set ray generation info
    context->setEntryPointCount(1);
    context->setRayTypeCount(1);

    // set programs

    context->setRayGenerationProgram(0, programs->get("global_raygen"));
    context->setExceptionProgram(0, programs->get("global_exception"));
    context->setMissProgram(0, programs->get("global_miss"));

    // create pixel buffer
    pixel_buffer = context->createBuffer(RT_BUFFER_OUTPUT);
    // set to rgba
    pixel_buffer->setFormat(RT_FORMAT_UNSIGNED_BYTE4);
    pixel_buffer->setSize(width, height);
    context["sysOutputBuffer"]->set(pixel_buffer);

    // global shader variables
    context["sysColorBackground"]->setFloat(1.0, 0.0, 0.0);
}

void Renderer::initWorld() {
    // create camera

    camera    = new Camera(0.0f, 0.5f, 90.0f, width, height);
    resources = new Resources(context, programs);

    // optix acceleration
    optix::Acceleration root = context->createAcceleration(std::string("NoAccel"));

    optix::Group groupRoot = context->createGroup();
    groupRoot->setAcceleration(root);
    groupRoot->setChildCount(0);

    context["sysRootObject"]->set(groupRoot);

    // read world description

    // create Geometrys and Materials

    // load into acceleration structure

    // send to GPU
}

void Renderer::resize(int width, int height) {
    this->width  = width;
    this->height = height;
    pixel_buffer->setSize(width, height);
    camera->setViewport(width, height);
}

void Renderer::launch() {
    bool set = camera->loadParameters(context);

    printf("set camera parameters (%s)\n", set ? "true" : "false");
    printf("launched!\n");

    try {
        context->launch(0, width, height);
    } catch (const optix::Exception& ex) {
        printf("launch Error: %d (%s)\n", ex.getErrorCode(), ex.getErrorString().c_str());
    }
}
