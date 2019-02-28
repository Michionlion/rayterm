# `RayTerm`

A Ray-Tracing Graphics Engine for XTerm-like Terminals


This repository is the GPU implementation of `RayTerm`. There is also an
incomplete CPU implementation, available [here](https://github.com/Michionlion/rayterm-cpu).

## Development

### Dependencies

`RayTerm` requires the [OptiX 6.0.0 SDK](http://raytracing-docs.nvidia.com/optix_6_0/index.html)
and [CUDA Toolkit 10.0](https://developer.nvidia.com/cuda-toolkit-archive) to
compile. It assumes the OptiX SDK is installed to `/opt/optix`, and the CUDA
Toolkit to `/opt/cuda`.
