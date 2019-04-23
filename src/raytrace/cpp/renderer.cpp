#include "renderer.h"
#include <optix.h>
#include <optix_world.h>
#include <iostream>
#include <string>
#include <vector>
#include "build_variables.h"
#include "programs.h"
#include "resources.h"

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
    raw_buffer = context->createBuffer(RT_BUFFER_OUTPUT);
    // set to rgba
    raw_buffer->setFormat(RT_FORMAT_UNSIGNED_BYTE4);
    raw_buffer->setSize(width, height);
    context["sysOutputBuffer"]->set(raw_buffer);

    context["sysNumSamples"]->setUint(samples);

    // global shader variables
    context["sysColorSky"]->setFloat(1, 1, 1);
    context["sysColorGround"]->setFloat(0.3, 0.5, 1);

    // setup post processing

    // TODO: fix denoiser library loading issues, add albedo buffer
    // optix::PostprocessingStage denoiser =
    // context->createBuiltinPostProcessingStage("DLDenoiser");
    // denoiser["input_buffer"]->set(raw_buffer);
    // denoiser["input_albedo_buffer"]->set(albedo_buffer);
    // denoiser["input_normal_buffer"]->set(normal_buffer);
    // denoiser["output_buffer"]->set(final_buffer);

    launch_stages = context->createCommandList();
    launch_stages->appendLaunch(0, width, height);
    // launch_stages->appendPostprocessingStage(denoiser, width, height);
    launch_stages->finalize();
}

void Renderer::initWorld() {
    // create camera

    camera = new Camera(0.0f, 0.6f, 74.0f, width, height);
    camera->setPosition(2.5, 2, 0);
    resources = new Resources(context, programs);

    optix::Group groupRoot = context->createGroup();
    groupRoot->setAcceleration(context->createAcceleration("Trbvh"));

    // sample world (monkey.obj and diffuse.mat)

    int monkeyObj     = resources->loadObjFile("monkey.obj");
    int coneObj       = resources->loadObjFile("cone.obj");
    int planeObj      = resources->loadObjFile("plane.obj");
    int sphereObj     = resources->loadObjFile("sphere.obj");
    int clayMat       = resources->loadMatFile("diffuse_clay.mat");
    int greenMat      = resources->loadMatFile("diffuse_green.mat");
    int blueMat       = resources->loadMatFile("diffuse_blue.mat");
    int mirrorMat     = resources->loadMatFile("mirror.mat");
    int glossyRedMat  = resources->loadMatFile("glossy_red.mat");
    int glossyGrayMat = resources->loadMatFile("glossy_gray.mat");
    int glassMat      = resources->loadMatFile("glass.mat");

    optix::GeometryGroup monkey1 = resources->createGeometryGroup(monkeyObj, glossyGrayMat,
        optix::Matrix4x4::translate(optix::make_float3(-2, 1, 2.5)) *
            optix::Matrix4x4::rotate(M_PI / 3, optix::make_float3(0, 1, 0)));
    optix::GeometryGroup monkey2 = resources->createGeometryGroup(
        monkeyObj, greenMat, optix::Matrix4x4::translate(optix::make_float3(-1.5, 2, -2.5)));
    optix::GeometryGroup cone  = resources->createGeometryGroup(coneObj, clayMat,
        optix::Matrix4x4::translate(optix::make_float3(0, 1, 0.5)) *
            optix::Matrix4x4::rotate(M_PI * 1.2f, optix::make_float3(1, 0, 0)));
    optix::GeometryGroup plane = resources->createGeometryGroup(
        planeObj, greenMat, optix::Matrix4x4::scale(optix::make_float3(10.0)));
    optix::GeometryGroup mirror  = resources->createGeometryGroup(planeObj, mirrorMat,
        optix::Matrix4x4::rotate(M_PI / 2, optix::make_float3(1, 0, 0)) *
            optix::Matrix4x4::rotate(M_PI / 3, optix::make_float3(0, 0, 1)) *
            optix::Matrix4x4::translate(optix::make_float3(-2, 4, -2)) *
            optix::Matrix4x4::scale(optix::make_float3(7.0)));
    optix::GeometryGroup sphere1 = resources->createGeometryGroup(sphereObj, blueMat,
        optix::Matrix4x4::scale(optix::make_float3(1.5)) *
            optix::Matrix4x4::translate(optix::make_float3(-1.5, 0.5, -1.5)));
    optix::GeometryGroup sphere2 = resources->createGeometryGroup(sphereObj, glassMat,
        optix::Matrix4x4::translate(optix::make_float3(-0.7, 1.65, -0.85)) *
            optix::Matrix4x4::scale(optix::make_float3(0.6)));

    groupRoot->addChild(monkey1);
    groupRoot->addChild(monkey2);
    groupRoot->addChild(cone);
    groupRoot->addChild(plane);
    groupRoot->addChild(mirror);
    groupRoot->addChild(sphere1);
    groupRoot->addChild(sphere2);

    // read world description

    // create Geometrys and Materials

    // load into acceleration structure

    // send to GPU
    context["sysRootObject"]->set(groupRoot);
}

void Renderer::resize(unsigned int width, unsigned int height) {
    this->width  = width;
    this->height = height;
    raw_buffer->setSize(width, height);
    camera->setViewport(width, height);
}

void Renderer::launch() {
    try {
        // bool set = camera->loadParameters(context);
        camera->loadParameters(context);

        // printf("set camera parameters (%s)\n", set ? "true" : "false");
        // printf("launched!\n");
        launch_stages->execute();
    } catch (const optix::Exception& ex) {
        printf("launch Error: %d (%s)\n", ex.getErrorCode(), ex.getErrorString().c_str());
    }
}
