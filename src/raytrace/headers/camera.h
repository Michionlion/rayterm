#ifndef _RAYTERM_CAMERA_H_
#define _RAYTERM_CAMERA_H_

#include <optix.h>
#include <optix_world.h>
// #include <optixu/optixpp_namespace.h>
// #include <optixu/optixu_math_namespace.h>

class Camera {
    int width;
    int height;
    float aspect_ratio;
    bool changed = false;

    // can be [0.0f, 1.0f], from positive x-axis 360 degrees around the horizon
    float phi;
    // can be [0.0f, 1.0f], from negative to positive y-axis
    float theta;
    // vertical field of view in degrees
    float fov;

    optix::float3 cameraPosition;
    optix::float3 cameraU;
    optix::float3 cameraV;
    optix::float3 cameraW;

   public:
    Camera(float phi, float theta, float fov, int width, int height) : fov(fov) {
        cameraPosition = optix::make_float3(0.0f, 0.0f, 0.0f);
        cameraU        = optix::make_float3(1.0f, 0.0f, 0.0f);
        cameraV        = optix::make_float3(0.0f, 1.0f, 0.0f);
        cameraW        = optix::make_float3(0.0f, 0.0f, -1.0f);

        setLook(phi, theta);
        setViewport(width, height);
    }
    ~Camera() = default;

    void setViewport(int w, int h);
    void setPosition(optix::float3 position);
    void setPosition(float x, float y, float z) { setPosition(optix::make_float3(x, y, z)); }
    void setLook(float p, float t);

    float getAspectRatio() const { return aspect_ratio; }

    // returns true if parameters were loaded, false if no need
    bool loadParameters(optix::Context context);
};

#endif
