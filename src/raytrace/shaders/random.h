#ifndef _RAYTERM_SHADER_RANDOM_H_
#define _RAYTERM_SHADER_RANDOM_H_

#include <optix.h>
#include <optix_world.h>
#include <optixu/optixu_math_namespace.h>

struct RandomGenerator {
    unsigned long long state;

    __host__ __device__ __inline__ void init(unsigned long long seed) {
        state = seed;
        // warmup
        // this->get();
        // this->get();
        // this->get();
    }

    // get the next random number in the sequence
    // based on drand48
    __host__ __device__ __inline__ float get() {
        const unsigned long long a    = 0x5DEECE66DULL;
        const unsigned long long c    = 0xBULL;
        const unsigned long long mask = 0xFFFFFFFFFFFFULL;
        state                         = a * state + c;
        return float((state & mask) / float(mask + 1ULL));
    }
};

static __host__ __device__ __inline__ optix::float3 random_in_usphere(RandomGenerator &generator) {
    optix::float3 vec = optix::make_float3(0.0f);
    optix::float3 one = make_float3(1.0f);
    do {
        vec = 2.0f * optix::make_float3(generator.get(), generator.get(), generator.get()) - one;
    } while (optix::length(vec) > 1.0);
    return vec;
}

static __host__ __device__ __inline__ optix::float3 random_in_uhemisphere(
    RandomGenerator &generator, const optix::float3 &normal) {
    optix::float3 vec = optix::make_float3(0.0f);
    optix::float3 one = make_float3(1.0f);
    do {
        vec = 2.0f * optix::make_float3(generator.get(), generator.get(), generator.get()) - one;
    } while (optix::length(vec) > 1.0 || optix::dot(normal, vec) <= 0);
    return vec;
}

#endif
