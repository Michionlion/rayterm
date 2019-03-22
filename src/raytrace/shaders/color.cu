#include <optix.h>
#include <optix_world.h>
#include <optixu/optixu_math_namespace.h>
#include "config.h"
#include "payload.h"

rtDeclareVariable(optix::Ray, theRay, rtCurrentRay, );
rtDeclareVariable(RayPayload, thePayload, rtPayload, );

rtDeclareVariable(optix::float3, varAttenuation, , );

RT_PROGRAM void closest_hit() { thePayload.radiance = varAttenuation; }

RT_PROGRAM void any_hit() {}
