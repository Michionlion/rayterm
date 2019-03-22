#include <optix.h>
#include <optixu/optixu_math_namespace.h>
#include "config.h"
#include "payload.h"

rtDeclareVariable(optix::Ray, theRay, rtCurrentRay, );
rtDeclareVariable(RayPayload, thePayload, rtPayload, );
rtDeclareVariable(optix::float3, sysColorSky, , );
rtDeclareVariable(optix::float3, sysColorGround, , );

RT_PROGRAM void miss_gradient(void) {
    const float t       = theRay.direction.y * 0.5f + 0.5f;
    thePayload.radiance = optix::lerp(sysColorGround, sysColorSky, t);
}
