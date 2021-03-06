#include <optix.h>
#include <optix_world.h>
#include <optixu/optixu_math_namespace.h>
#include "config.h"
#include "payload.h"
#include "random.h"

#define MAX_DEPTH 5

// Note, the nomenclature used in the device code of all optixIntroduction samples
// follows some simple rules using prefixes to help indicating the scope and meaning:
//
// "sys" = renderer "system"-wide variables, defined at global context scope.
// "the" = variables with OptiX built-in semantic, like rtLaunchIndex, etc.
// "var" = "varyings" with developer defined attribute semantic, calculated by the intersection
// program. "par" = "parameter" variable held at some object scope, not at the global context scope.
//         (Exception to the last rule are the vertex "attributes" and "indices" held at Geometry
//         nodes.)

rtBuffer<uchar4, 2> sysOutputBuffer;  // RGBA

rtDeclareVariable(rtObject, sysRootObject, , );
rtDeclareVariable(uint, sysNumSamples, , );

rtDeclareVariable(uint2, theLaunchDim, rtLaunchDim, );
rtDeclareVariable(uint2, theLaunchIndex, rtLaunchIndex, );

rtDeclareVariable(float3, sysCameraPosition, , );
rtDeclareVariable(float3, sysCameraU, , );
rtDeclareVariable(float3, sysCameraV, , );
rtDeclareVariable(float3, sysCameraW, , );

// exposure + simple Reinhard tonemapper + gamma
RT_FUNCTION uchar4 display(float3 val) {
    // clamp
    // val.x = optix::fminf(optix::fmaxf(val.x, 0.0f), 1.0f);
    // val.y = optix::fminf(optix::fmaxf(val.y, 0.0f), 1.0f);
    // val.z = optix::fminf(optix::fmaxf(val.z, 0.0f), 1.0f);

    // do tonemap
    // val *= 1.0f;
    // const float burn_out = 1.0f;
    // val.x *= (1.0f + val.x * burn_out) / (1.0f + val.x);
    // val.y *= (1.0f + val.y * burn_out) / (1.0f + val.y);
    // val.z *= (1.0f + val.z * burn_out) / (1.0f + val.z);
    // // these WERE in z->y->x order
    // return make_uchar4(
    //     (unsigned char)(255.0 * fminf(powf(fmaxf(val.x, 0.0f), (float)(1.0 / 2.2)), 1.0f)),
    //     (unsigned char)(255.0 * fminf(powf(fmaxf(val.y, 0.0f), (float)(1.0 / 2.2)), 1.0f)),
    //     (unsigned char)(255.0 * fminf(powf(fmaxf(val.z, 0.0f), (float)(1.0 / 2.2)), 1.0f)), 255);

    // do bt_709
    uchar4 output = make_uchar4(0u, 255u, 0u, 255u);
    if (val.x < 0.018) {
        output.x = (unsigned char)(val.x * 4.5 * 255.0);
    } else {
        output.x = (unsigned char)(255.0 * (1.099 * pow(val.x, 0.45) - 0.099));
    }
    if (val.y < 0.018) {
        output.y = (unsigned char)(val.y * 4.5 * 255.0);
    } else {
        output.y = (unsigned char)(255.0 * (1.099 * pow(val.y, 0.45) - 0.099));
    }
    if (val.z < 0.018) {
        output.z = (unsigned char)(val.z * 4.5 * 255.0);
    } else {
        output.z = (unsigned char)(255.0 * (1.099 * pow(val.z, 0.45) - 0.099));
    }
    // output.x = (unsigned char)(val.x * 255.0);
    // output.y = (unsigned char)(val.y * 255.0);
    // output.z = (unsigned char)(val.z * 255.0);
    return output;
}

// Entry point
RT_PROGRAM void raygen() {
    // the launch dimension is the screen size
    const float2 screen = make_float2(theLaunchDim);
    // The launch index is the current pixel coordinate
    // Note that launchIndex = (0, 0) is the bottom left corner of the image
    const float2 pixel = make_float2(theLaunchIndex);

    RayPayload payload;
    // seed this pixels' random number generator
    RandomGenerator generator;
    generator.init(
        (unsigned long long)(screen.x * pixel.y) ^ (unsigned long long)(screen.y * pixel.x));

    payload.rand = generator;

    optix::float3 accum = optix::make_float3(0);

    for (int sample = 0; sample < sysNumSamples; sample++) {
        payload.radiance = make_float3(0);
        payload.depth    = MAX_DEPTH;
        // sample the pixel with subpixel jitter
        const float2 sample_pos = pixel + make_float2(generator.get(), generator.get());

        // normalized device coordinates in range [-1, 1]
        const float2 ndc = (sample_pos / screen) * 2.0f - 1.0f;

        const float3 origin = sysCameraPosition;

        // must be normalized
        const float3 direction =
            optix::normalize(ndc.x * sysCameraU + ndc.y * sysCameraV + sysCameraW);

        optix::Ray ray = optix::make_Ray(origin, direction, 0, 0.0f, RT_DEFAULT_MAX);

        // start the ray traversal at the root object
        rtTrace(sysRootObject, ray, payload);

        accum += payload.radiance;
    }

    sysOutputBuffer[theLaunchIndex] = display(accum / sysNumSamples);
}
