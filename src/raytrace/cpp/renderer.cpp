#include "renderer.h"
#include <iostream>
#include <string>
#include <vector>

#define HANDLE_EXCEPTIONS 1
#define STACK_SIZE 1024

// PROGRAM_DIRECTORY is a directory to look in for .ptx files
// I assume if no directory is given, the directory is build/ptx
#ifndef PROGRAM_DIRECTORY
#define PROGRAM_DIRECTORY "build/ptx"
#endif

// PROGRAM_LIST is a string array literal of .ptx file names
// I assume that if no list is given, exception and raygen exist
#ifndef PROGRAM_LIST
#define PROGRAM_LIST \
    { "exception", "raygen" }
#endif

void Renderer::initContext() {
    uint32_t device_count;
    rtDeviceGetDeviceCount(&device_count);
    if (!device_count) {
        fprintf(stderr, "A supported NVIDIA GPU could not be found.\n");
        exit(1);
    }
    context = optix::Context::create();

    // specify context details
    context->setStackSize(STACK_SIZE);

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
    std::vector<std::string> program_names = std::vector<std::string> PROGRAM_LIST;

    for (const auto& name : program_names) {
        std::string path =
            std::string(PROGRAM_DIRECTORY) + std::string("/") + name + std::string(".ptx");
        try {
            programs[name] = context->createProgramFromPTXFile(path, name);
            printf("Compiled '%s' to '%s'\n", path.c_str(), name.c_str());
            programs[name]->validate();
        } catch (optix::Exception& ex) {
            printf("Error compiling '%s': %d (%s)\n", name.c_str(), ex.getErrorCode(),
                ex.getErrorString().c_str());
        }
    }
}

void Renderer::initOptiX() {
    // set ray generation info
    context->setEntryPointCount(1);
    context->setRayTypeCount(1);

    // set programs
    // context->setRayGenerationProgram(0, programs["raygen"]);
    // context->setExceptionProgram(0, programs["exception"]);

    std::map<std::string, optix::Program>::const_iterator it = programs.find("raygen");
    if (it == programs.end()) {
        printf("Error: could not find required 'raygen' ptx program\n");
        exit(1);
    }
    context->setRayGenerationProgram(0, it->second);

    it = programs.find("exception");
    if (it == programs.end()) {
        printf("Error: could not find required 'exception' ptx program\n");
        exit(1);
    }
    context->setExceptionProgram(0, it->second);

    it = programs.find("miss_gradient");
    if (it == programs.end()) {
        printf("Error: could not find required 'miss_gradient' ptx program\n");
        exit(1);
    }
    context->setMissProgram(0, it->second);

    // create pixel buffer
    pixel_buffer = context->createBuffer(RT_BUFFER_OUTPUT);
    // set to rgba
    pixel_buffer->setFormat(RT_FORMAT_FLOAT4);
    pixel_buffer->setSize(width, height);
    context["sysOutputBuffer"]->set(pixel_buffer);

    // global shader variables
    context["sysColorBackground"]->setFloat(1.0, 0.0, 0.0);
}

void Renderer::initWorld() {
    // create camera

    camera = new Camera(0.0f, 0.5f, 90.0f, width, height);

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
