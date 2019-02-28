# `RayTerm`

A Ray-Tracing Graphics Engine for XTerm-like Terminals


This repository is the GPU implementation of `RayTerm`. There is also an
incomplete CPU implementation, available [here](https://github.com/Michionlion/rayterm-cpu).

## Development

### Dependencies

`RayTerm` requires the following to compile:
* [OptiX 6.0.0 SDK](http://raytracing-docs.nvidia.com/optix_6_0/index.html)
  (Installed to `/opt/optix`)
* [CUDA Toolkit 10.0](https://developer.nvidia.com/cuda-toolkit-archive)
  (Installed to `/opt/cuda`)
* [MDL SDK](https://developer.nvidia.com/mdl-sdk)
  (Installed to `/opt/mdl`)

The required linking and PATH additions should be made, as defined in the
respective documentation. Notably, the `nvcc` compiler must be available, and
the OptiX and MDL `.so` libraries must be linkable.

### Testing

For manual rendering comparisons, [blender](https://www.blender.org/) is used.
