#include <optix.h>
#include <optix_world.h>
#include <optixu/optixu_math_namespace.h>
#include "config.h"
#include "payload.h"
#include "random.h"

// attribute program
// very slightly inspired from OptiX SDK example 'optixGeometryTriangles'
// (there's literally no documentation on GeometryTriangles)

rtDeclareVariable(rtObject, sysRootObject, , );
rtDeclareVariable(float2, barycentrics, attribute rtTriangleBarycentrics, );
rtDeclareVariable(float, distance, rtIntersectionDistance, );

rtBuffer<float3> vbuf;
rtBuffer<float3> nbuf;
rtBuffer<int3> vidx;
rtBuffer<int3> nidx;

rtDeclareVariable(optix::Ray, theRay, rtCurrentRay, );
rtDeclareVariable(RayPayload, thePayload, rtPayload, );

rtDeclareVariable(optix::float3, varAttenuation, , );
rtDeclareVariable(float, varRoughness, , );

RT_PROGRAM void closest_hit() {
    thePayload.depth -= 1;
    if (thePayload.depth > 0) {
        // calculate interpolated vertex (shading) normal
        const optix::int3 n_idx = nidx[rtGetPrimitiveIndex()];
        const optix::float3 normal =
            optix::normalize(nbuf[n_idx.y] * barycentrics.x + nbuf[n_idx.z] * barycentrics.y +
                             nbuf[n_idx.x] * (1.0f - barycentrics.x - barycentrics.y));

        optix::float3 reflect_dir = optix::reflect(theRay.direction, normal);
        optix::float3 random_dir = optix::normalize(random_in_uhemisphere(thePayload.rand, normal));
        optix::float3 ref_dir    = optix::lerp(reflect_dir, random_dir, varRoughness);
        // should test to see if ref_dir is below geometry with geometric normal -- probably doesn't
        // really matter
        optix::float3 hit_pos = theRay.origin + distance * theRay.direction;
        optix::Ray bounce     = optix::make_Ray(hit_pos, ref_dir, 0, 0.00001f, RT_DEFAULT_MAX);

        rtTrace(sysRootObject, bounce, thePayload);
        thePayload.radiance = varAttenuation * thePayload.radiance;
    } else {
        thePayload.radiance = optix::make_float3(0);
    }
}

RT_PROGRAM void any_hit() {}
