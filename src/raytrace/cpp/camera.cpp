#include "camera.h"
#include <optix.h>
#include <optix_world.h>
#include <cmath>

void Camera::setViewport(int w, int h) {
    if (width != w || height != h) {
        // never drop to zero viewport size
        width        = (w) ? w : 1;
        height       = (h) ? h : 1;
        aspect_ratio = float(width) / float(height);
        changed      = true;
    }
}

void Camera::setPosition(optix::float3 position) {
    if (cameraPosition.x != position.x || cameraPosition.y != position.y ||
        cameraPosition.z != position.z) {
        cameraPosition = position;
        changed        = true;
    }
}

void Camera::setLook(float p, float t) {
    if (phi != p || theta != t) {
        phi     = p;
        theta   = t;
        changed = true;
    }
}

bool Camera::loadParameters(optix::Context context) {
    if (!changed) {
        return false;
    }

    // Recalculate the camera parameters.
    const float cosPhi   = cosf(phi * 2.0f * M_PIf);
    const float sinPhi   = sinf(phi * 2.0f * M_PIf);
    const float cosTheta = cosf(theta * M_PIf);
    const float sinTheta = sinf(theta * M_PIf);

    // "normal", unit vector from origin to spherical coordinates
    // (phi, theta)
    optix::float3 normal = optix::make_float3(cosPhi * sinTheta, -cosTheta, -sinPhi * sinTheta);

    // fov is in the range [1.0f, 179.0f].
    float tanFov = tanf((fov * 0.5f) * M_PIf / 180.0f);

    // "tangent"
    cameraU = aspect_ratio * optix::make_float3(-sinPhi, 0.0f, -cosPhi) * tanFov;
    // "bitangent"
    cameraV = optix::make_float3(cosTheta * cosPhi, sinTheta, cosTheta * -sinPhi) * tanFov;
    // "-normal" to look at the center.
    cameraW = -normal;

    context["sysCameraPosition"]->setFloat(cameraPosition);
    context["sysCameraU"]->setFloat(cameraU);
    context["sysCameraV"]->setFloat(cameraV);
    context["sysCameraW"]->setFloat(cameraW);

    changed = false;
    return true;
}
