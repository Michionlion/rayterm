#include "config.h"

#include <optix.h>
#include <optixu/optixu_math_namespace.h>

#include "payload.h"

rtDeclareVariable(optix::Ray, theRay, rtCurrentRay, );

rtDeclareVariable(RayPayload, thePayload, rtPayload, );

RT_PROGRAM void miss_gradient(void) {
    const float t       = theRay.direction.y * 0.5f + 0.5f;
    thePayload.radiance = optix::lerp(make_float3(0.0f), make_float3(1.0f), t);
}
