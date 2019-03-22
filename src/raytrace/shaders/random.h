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


static __host__ __device__ __inline__ optix::float3 random_in_uhemisphere(
    RandomGenerator &generator, const optix::float3 &normal) {
    optix::float3 sample = optix::make_float3(0);

    // cosine_sample_hemisphere generates z+ centered hemisphere distribution (not y+)
    optix::cosine_sample_hemisphere(generator.get(), generator.get(), sample);

    optix::float3 Nt, Nb;

    if (std::fabs(normal.x) > std::fabs(normal.y)) {
        Nt = optix::make_float3(normal.z, 0, -normal.x) /
             sqrtf(normal.x * normal.x + normal.z * normal.z);
    } else {
        Nt = optix::make_float3(0, -normal.z, normal.y) /
             sqrtf(normal.y * normal.y + normal.z * normal.z);
    }
    Nb = optix::cross(normal, Nt);

    // convert to normal's space directly
    // math adapted from below, which uses y+ hemisphere distribution
    // https://www.scratchapixel.com/lessons/3d-basic-rendering/global-illumination-path-tracing
    return optix::make_float3(sample.x * Nb.x + sample.z * normal.x + sample.y * Nt.x,
        sample.x * Nb.y + sample.z * normal.y + sample.y * Nt.y,
        sample.x * Nb.z + sample.z * normal.z + sample.y * Nt.z);
}

#endif
