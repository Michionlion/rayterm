#include <optix.h>
#include "config.h"

rtBuffer<uchar4, 2> sysOutputBuffer;

rtDeclareVariable(uint2, theLaunchIndex, rtLaunchIndex, );

RT_PROGRAM void exception() {
#if USE_DEBUG_EXCEPTIONS
    const unsigned int code = rtGetExceptionCode();
    if (RT_EXCEPTION_USER <= code) {
        rtPrintf("User exception %d at (%d, %d)\n", code - RT_EXCEPTION_USER, theLaunchIndex.x,
            theLaunchIndex.y);
    } else {
        rtPrintf("Exception code 0x%X at (%d, %d)\n", code, theLaunchIndex.x, theLaunchIndex.y);
    }
    // RGBA32F super magenta as error color (makes sure this isn't accumulated away in a progressive
    // renderer).
    sysOutputBuffer[theLaunchIndex] = make_uchar4(255, 0, 255, 255);
#endif
}
