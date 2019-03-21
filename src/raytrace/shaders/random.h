#ifndef _RAYTERM_SHADER_RANDOM_H_
#define _RAYTERM_SHADER_RANDOM_H_
/*
 * Copyright (c) 2018, NVIDIA CORPORATION. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of NVIDIA CORPORATION nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <optix.h>
#include <optix_world.h>
#include <optixu/optixu_math_namespace.h>

// their code

template <unsigned int N>
static __host__ __device__ __inline__ unsigned int tea(unsigned int val0, unsigned int val1) {
    unsigned int v0 = val0;
    unsigned int v1 = val1;
    unsigned int s0 = 0;

    for (unsigned int n = 0; n < N; n++) {
        s0 += 0x9e3779b9;
        v0 += ((v1 << 4) + 0xa341316c) ^ (v1 + s0) ^ ((v1 >> 5) + 0xc8013ea4);
        v1 += ((v0 << 4) + 0xad90777d) ^ (v0 + s0) ^ ((v0 >> 5) + 0x7e95761e);
    }

    return v0;
}

// Generate random unsigned int in [0, 2^24)
static __host__ __device__ __inline__ unsigned int lcg(unsigned int &prev) {
    const unsigned int LCG_A = 1664525u;
    const unsigned int LCG_C = 1013904223u;
    prev                     = (LCG_A * prev + LCG_C);
    return prev & 0x00FFFFFF;
}

static __host__ __device__ __inline__ unsigned int lcg2(unsigned int &prev) {
    prev = (prev * 8121 + 28411) % 134456;
    return prev;
}

// Generate random float in [0, 1)
static __host__ __device__ __inline__ float rnd(unsigned int &prev) {
    return ((float)lcg(prev) / (float)0x01000000);
}

static __host__ __device__ __inline__ unsigned int rot_seed(unsigned int seed, unsigned int frame) {
    return seed ^ frame;
}

// our code

// static __host__ __device__ __inline__ optix::float3 random_in_usphere(unsigned int &prev) {
//     optix::float3 vec = optix::make_float3(0.0f);
//     optix::float3 one = make_float3(1.0f);
//     do {
//         vec = 2.0f * optix::make_float3(prev = rnd(prev), prev = rnd(prev), prev = rnd(prev)) -
//         one;
//     } while (optix::length(vec) > 1.0);
//     return vec;
// }
//
// static __host__ __device__ __inline__ optix::float3 random_in_uhemisphere(
//     unsigned int &prev, const optix::float3 &normal) {
//     optix::float3 vec = optix::make_float3(0.0f);
//     optix::float3 one = make_float3(1.0f);
//     do {
//         vec = 2.0f * optix::make_float3(prev = rnd(prev), prev = rnd(prev), prev = rnd(prev)) -
//         one;
//     } while (optix::length(vec) > 1.0 || optix::dot(normal, vec) <= 0);
//     return vec;
// }

#endif
