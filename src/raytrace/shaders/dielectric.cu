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
rtDeclareVariable(float, varIndexOfRefraction, , );

RT_PROGRAM void closest_hit() {
    thePayload.depth -= 1;
    if (thePayload.depth > 0) {
        // calculate interpolated vertex (shading) normal
        const optix::int3 n_idx = nidx[rtGetPrimitiveIndex()];
        const optix::float3 normal =
            optix::normalize(nbuf[n_idx.y] * barycentrics.x + nbuf[n_idx.z] * barycentrics.y +
                             nbuf[n_idx.x] * (1.0f - barycentrics.x - barycentrics.y));

        optix::Ray bounce;
        optix::float3 outward_normal;
        optix::float3 reflected = optix::reflect(theRay.direction, normal);
        float ni_over_nt;
        optix::float3 refracted;
        float reflect_prob;
        float cosine;

        if (optix::dot(theRay.direction, normal) > 0.0f) {
            outward_normal = -normal;
            ni_over_nt     = varIndexOfRefraction;
            cosine         = optix::dot(theRay.direction, normal) / optix::length(theRay.direction);
            cosine         = sqrtf(
                1.0f - varIndexOfRefraction * varIndexOfRefraction * (1.0f - cosine * cosine));
        } else {
            outward_normal = normal;
            ni_over_nt     = 1.0f / varIndexOfRefraction;
            cosine = -optix::dot(theRay.direction, normal) / optix::length(theRay.direction);
        }

        // do refraction (optix does something weird)
        optix::float3 incoming = optix::normalize(theRay.direction);
        float dt               = optix::dot(incoming, outward_normal);
        float discriminant     = 1.0f - ni_over_nt * ni_over_nt * (1 - dt * dt);
        if (discriminant > 0.0f) {
            refracted = ni_over_nt * (incoming - outward_normal * dt) -
                        outward_normal * sqrtf(discriminant);

            // calculate schlick approximation
            float r0     = (1.0f - varIndexOfRefraction) / (1.0f + varIndexOfRefraction);
            r0           = r0 * r0;
            reflect_prob = r0 + (1.0f - r0) * pow((1.0f - cosine), 5.0f);

        } else {
            reflect_prob = 1.0f;
        }

        optix::float3 hit_pos = theRay.origin + distance * theRay.direction;
        if (thePayload.rand.get() < reflect_prob) {
            bounce = optix::make_Ray(hit_pos, reflected, 0, 0.00001f, RT_DEFAULT_MAX);
        } else {
            bounce = optix::make_Ray(hit_pos, refracted, 0, 0.00001f, RT_DEFAULT_MAX);
        }

        rtTrace(sysRootObject, bounce, thePayload);
        thePayload.radiance = varAttenuation * thePayload.radiance;
    } else {
        thePayload.radiance = optix::make_float3(0);
    }
}

RT_PROGRAM void any_hit() {}
