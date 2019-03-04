#ifndef _RAYTERM_SHADER_CONFIG_H_
#define _RAYTERM_SHADER_CONFIG_H_
// From the OptiX Header:
// Unless compatibility with SM_10 is needed, new code should use this define and rely on the new
// templated version of rtCallableProgram.
#define RT_USE_TEMPLATED_RTCALLABLEPROGRAM 1

// DAR Prevent that division by very small floating point values results in huge values, for example
// dividing by pdf.
#define DENOMINATOR_EPSILON 1.0e-6f

// 0 == Disable all OptiX exceptions, rtPrintfs and rtAssert functionality. (Benchmark only in this
// mode!) 1 == Enable  all OptiX exceptions, rtPrintfs and rtAssert functionality. (Really only for
// debugging, big performance hit!)
#define USE_DEBUG_EXCEPTIONS 1

#include <optix.h>
#include <optixu/optixu_math_namespace.h>

#ifndef RT_FUNCTION
#define RT_FUNCTION __forceinline__ __device__
#endif

#endif
