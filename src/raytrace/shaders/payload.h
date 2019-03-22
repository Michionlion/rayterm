#ifndef _RAYTERM_SHADER_PAYLOAD_H_
#define _RAYTERM_SHADER_PAYLOAD_H_

#include "config.h"
#include "random.h"

struct RayPayload {
    optix::float3 radiance;
    RandomGenerator rand;
    unsigned char depth;
};

#endif
