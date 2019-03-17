# `RayTerm`

![Travis CI Build Status Image](https://travis-ci.com/Michionlion/rayterm.svg?branch=master)

A Ray-Tracing Graphics Engine for XTerm-like Terminals

This repository is the GPU implementation of `RayTerm`. There is also an
incomplete CPU implementation, available [here](https://github.com/Michionlion/rayterm-cpu).

## Development

### Dependencies

`RayTerm` requires the following to compile:

* [OptiX 6.0.0 SDK](http://raytracing-docs.nvidia.com/optix_6_0/index.html)
  (Installed to `lib/optix`)
* [CUDA Toolkit 10.0](https://developer.nvidia.com/cuda-toolkit-archive)
  (Installed to `lib/cuda`)
* [MDL SDK](https://developer.nvidia.com/mdl-sdk)
  (Installed to `lib/mdl`)
* [GoogleTest](https://github.com/google/googletest)
  (Installed to `lib/googletest`)

These libraries are all installed automatically by running `gradle deps`,
although installation may take some time as many of them are large. See
`gradle.properties` to change the default expected installation locations, if
you wish to use system packages. Additionally, the required linking and PATH
additions should be made, as defined in the respective documentation, if these
are installed manually. Notably, the OptiX and MDL `.so` libraries must be
linkable.

The `gcc` toolchain is used to compile `RayTerm`. We specifically require
`g++-7`, and by default searches `/usr/bin/` for it.

Linting dependencies such as [mdl](https://github.com/markdownlint/markdownlint)
and [proselint](https://github.com/amperser/proselint/) ensure documentation is
well written; they should be installed before running `gradle lint`.

### Testing

For manual rendering comparisons, [blender](https://www.blender.org/) is used.

### Build System

`RayTerm` uses the [Gradle](https://gradle.org/) to automate compilation and
testing. To build `RayTerm`, run the command `gradle build`. To run unit tests,
run `gradle test`. To lint documentation and source code (where applicable),
issue `gradle lint`.

Custom scripts are also used to simplify and speed up the development process:
`test.sh` compiles only what is needed for a `ppm` render test, and `run.sh`
compiles the nessescary components for an `rtexplore` executable. Both are
configurable with simple command-line arguments.
