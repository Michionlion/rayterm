#include <optix.h>
#include <optix_world.h>
#include <optixu/optixu_math_namespace.h>
#include "config.h"
#include "payload.h"
#include "random.h"

rtDeclareVariable(float2, barycentrics, attribute rtTriangleBarycentrics, );

rtBuffer<float3> nbuf;
rtBuffer<int3> nidx;

rtDeclareVariable(optix::Ray, theRay, rtCurrentRay, );
rtDeclareVariable(RayPayload, thePayload, rtPayload, );

RT_PROGRAM void closest_hit() {
    // calculate interpolated vertex (shading) normal
    const optix::int3 n_idx = nidx[rtGetPrimitiveIndex()];
    const optix::float3 normal =
        optix::normalize(nbuf[n_idx.y] * barycentrics.x + nbuf[n_idx.z] * barycentrics.y +
                         nbuf[n_idx.x] * (1.0f - barycentrics.x - barycentrics.y));
    thePayload.radiance = normal * 0.5f + 0.5f;
}

RT_PROGRAM void any_hit() {}
