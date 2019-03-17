# `RayTerm`

![Travis CI Build Status Image](https://travis-ci.com/Michionlion/rayterm.svg?branch=master)

A Ray-Tracing Graphics Engine for XTerm-like Terminals

This repository is the GPU implementation of `RayTerm`. There is also an
incomplete CPU implementation, available [here](https://github.com/Michionlion/rayterm-cpu).

## Development

### Dependencies

`RayTerm` requires the following to compile:

* [OptiX 6.0.0 SDK](http://raytracing-docs.nvidia.com/optix_6_0/index.html)
  (Installed to `/opt/optix` by default) ([Travis Repository](https://github.com/Michionlion/optix))
* [CUDA Toolkit 10.0](https://developer.nvidia.com/cuda-toolkit-archive)
  (Installed to `/opt/cuda` by default) ([Travis Repository](https://github.com/Michionlion/cuda))
* [MDL SDK](https://developer.nvidia.com/mdl-sdk)
  (Installed to `/opt/mdl` by default) ([Travis Repository](https://github.com/Michionlion/mdl))

The required linking and PATH additions should be made, as defined in the
respective documentation. Notably, the OptiX and MDL `.so` libraries must
be linkable. See `gradle.properties` to change the default expected installation
locations. Travis uses the given repositories to download the needed headers,
libraries, and binaries when building and testing this project.

Additional dependencies such as [Google Test](https://github.com/google/googletest)
are vendored as submodules in this repository, and can be automatically updated
and built by issuing `gradle deps`. This should be done before building anything.

### Testing

For manual rendering comparisons, [blender](https://www.blender.org/) is used.

### Build System

`RayTerm` uses the [Gradle](https://gradle.org/) to automate compilation and
testing. To build `RayTerm`, run the command `gradle build`. To run unit tests,
run `gradle test`. To lint documentation and source code (where applicable),
issue `gradle lint`.
