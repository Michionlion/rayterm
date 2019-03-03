#include "renderer.h"
#include <iostream>
#include <vector>

#define HANDLE_EXCEPTIONS true
#define STACK_SIZE 1024

void Renderer::resize(int width, int height) {
    this->width  = width;
    this->height = height;
    pixel_buffer->setSize(width, height);
}

void Renderer::initOptiX() {
    context = optix::Context::create();

    // specify context details
    context->setEntryPointCount(1);
    context->setRayTypeCount(0);
    context->setStackSize(STACK_SIZE);

    std::vector<int> devices;
    devices.push_back(0);

    context->setDevices(devices.begin(), devices.end());

    printf("Device: %d\n", context->getEnabledDevices());

    // create pixel buffer
    pixel_buffer = context->createBuffer(RT_BUFFER_OUTPUT);
    // set to rgba
    pixel_buffer->setFormat(RT_FORMAT_FLOAT4);
    resize(width, height);

    context["sysOutputBuffer"]->set(pixel_buffer);

#if HANDLE_EXCEPTIONS
    context->setPrintEnabled(true);
    context->setExceptionEnabled(RT_EXCEPTION_ALL, true);
#endif
}

void Renderer::initPrograms() {
    try {
        programs["raygen"]    = context->createProgramFromPTXFile("ptx/raygen.ptx", "raygen");
        programs["exception"] = context->createProgramFromPTXFile("ptx/exception.ptx", "exception");
    } catch (optix::Exception& ex) {
        printf("initPrograms Error: %d (%s)\n", ex.getErrorCode(), ex.getErrorString().c_str());
    }
}

void Renderer::initWorld() {
    // read world description

    // create Geometrys and Materials

    // load into acceleration structure

    // send to GPU
}

void Renderer::launch() {
    printf("launched!\n");
    printSystemInfo();
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
