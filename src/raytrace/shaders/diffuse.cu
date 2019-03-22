#include "config.h"

#include <optix.h>
#include <optix_world.h>
#include <optixu/optixu_math_namespace.h>
#include "payload.h"
#include "random.h"

// attribute program
// very slightly inspired from OptiX SDK example 'optixGeometryTriangles'
// (there's literally no documentation on GeometryTriangles)

rtDeclareVariable(float2, barycentrics, attribute rtTriangleBarycentrics, );

rtDeclareVariable(float, distance, rtIntersectionDistance, );

rtBuffer<float3> vbuf;
rtBuffer<float3> nbuf;
rtBuffer<int3> vidx;
rtBuffer<int3> nidx;

rtDeclareVariable(optix::Ray, theRay, rtCurrentRay, );
rtDeclareVariable(RayPayload, thePayload, rtPayload, );

rtDeclareVariable(optix::float3, varColor, varColor, );

RT_PROGRAM void closest_hit() {
    const optix::int3 v_idx = vidx[rtGetPrimitiveIndex()];
    const optix::float3 v0  = vbuf[v_idx.x];
    const optix::float3 v1  = vbuf[v_idx.y];
    const optix::float3 v2  = vbuf[v_idx.z];
    // const optix::float3 Ng        = optix::cross(v1 - v0, v2 - v0);
    // const float3 geometric_normal = optix::normalize(Ng);

    const optix::int3 n_idx = nidx[rtGetPrimitiveIndex()];
    const optix::float3 shading_normal =
        optix::normalize(nbuf[n_idx.y] * barycentrics.x + nbuf[n_idx.z] * barycentrics.y +
                         nbuf[n_idx.x] * (1.0f - barycentrics.x - barycentrics.y));

    // optix::float3 dir = random_in_uhemisphere(thePayload.rand, geometric_normal);
    // thePayload.rand   = dir.z;

    // thePayload.radiance =
    //     make_float3(barycentrics.x, barycentrics.y, 1.0f - barycentrics.x - barycentrics.y);
    thePayload.radiance = shading_normal * 0.5f + 0.5f;
}

RT_PROGRAM void any_hit() {}
