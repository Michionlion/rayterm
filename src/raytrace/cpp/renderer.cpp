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
    context = optix::Context::create();

    // specify context details
    context->setStackSize(STACK_SIZE);

    std::vector<int> devices;
    devices.push_back(0);

    context->setDevices(devices.begin(), devices.end());

    printf("First Device: %d\n", context->getEnabledDevices()[0]);

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
    resize(width, height);
    context["sysOutputBuffer"]->set(pixel_buffer);  // NOLINT

    // global shader variables
    context["sysColorBackground"]->setFloat(1.0, 0.0, 0.0);  // NOLINT
}

void Renderer::initWorld() {
    // optix acceleration
    optix::Acceleration root = context->createAcceleration(std::string("NoAccel"));

    optix::Group groupRoot = context->createGroup();
    groupRoot->setAcceleration(root);
    groupRoot->setChildCount(0);

    context["sysRootObject"]->set(groupRoot);  // NOLINT

    // read world description

    // create Geometrys and Materials

    // load into acceleration structure

    // send to GPU
}

void Renderer::resize(int width, int height) {
    this->width  = width;
    this->height = height;
    pixel_buffer->setSize(width, height);
}

void Renderer::launch() {
    printf("launched!\n");

    try {
        context->launch(0, width, height);
    } catch (const optix::Exception& ex) {
        printf("launch Error: %d (%s)\n", ex.getErrorCode(), ex.getErrorString().c_str());
    }
}

void Renderer::printSystemInfo() {
    uint32_t numberOfDevices = 0;
    RT_CHECK_ERROR_NO_CONTEXT(rtDeviceGetDeviceCount(&numberOfDevices));

    for (uint32_t i = 0; i < numberOfDevices; i++) {
        char name[256];
        RT_CHECK_ERROR_NO_CONTEXT(
            rtDeviceGetAttribute(i, RT_DEVICE_ATTRIBUTE_NAME, sizeof(name), name));
        std::cout << "Device " << i << ": " << name << std::endl;

        int computeCapability[2] = {0, 0};
        RT_CHECK_ERROR_NO_CONTEXT(rtDeviceGetAttribute(i, RT_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY,
            sizeof(computeCapability), &computeCapability));
        std::cout << "  Compute Support: " << computeCapability[0] << "." << computeCapability[1]
                  << std::endl;

        RTsize totalMemory = 0;
        RT_CHECK_ERROR_NO_CONTEXT(rtDeviceGetAttribute(
            i, RT_DEVICE_ATTRIBUTE_TOTAL_MEMORY, sizeof(totalMemory), &totalMemory));
        std::cout << "  Total Memory: " << static_cast<uint64_t>(totalMemory) / 1000000000.0 << "GB"
                  << std::endl;

        int clockRate = 0;
        RT_CHECK_ERROR_NO_CONTEXT(
            rtDeviceGetAttribute(i, RT_DEVICE_ATTRIBUTE_CLOCK_RATE, sizeof(clockRate), &clockRate));
        std::cout << "  Clock Rate: " << clockRate << " kHz" << std::endl;

        int maxThreadsPerBlock = 0;
        RT_CHECK_ERROR_NO_CONTEXT(rtDeviceGetAttribute(i, RT_DEVICE_ATTRIBUTE_MAX_THREADS_PER_BLOCK,
            sizeof(maxThreadsPerBlock), &maxThreadsPerBlock));
        std::cout << "  Max. Threads per Block: " << maxThreadsPerBlock << std::endl;

        int smCount = 0;
        RT_CHECK_ERROR_NO_CONTEXT(rtDeviceGetAttribute(
            i, RT_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT, sizeof(smCount), &smCount));
        std::cout << "  Streaming Multiprocessor Count: " << smCount << std::endl;

        int executionTimeoutEnabled = 0;
        RT_CHECK_ERROR_NO_CONTEXT(
            rtDeviceGetAttribute(i, RT_DEVICE_ATTRIBUTE_EXECUTION_TIMEOUT_ENABLED,
                sizeof(executionTimeoutEnabled), &executionTimeoutEnabled));
        std::cout << "  Execution Timeout Enabled: " << executionTimeoutEnabled << std::endl;

        int maxHardwareTextureCount = 0;
        RT_CHECK_ERROR_NO_CONTEXT(
            rtDeviceGetAttribute(i, RT_DEVICE_ATTRIBUTE_MAX_HARDWARE_TEXTURE_COUNT,
                sizeof(maxHardwareTextureCount), &maxHardwareTextureCount));
        std::cout << "  Max. Hardware Texture Count: " << maxHardwareTextureCount << std::endl;
    }
}
