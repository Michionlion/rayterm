# `RayTerm`

## A Ray-Tracing Graphics Engine for XTerm-like Terminals

[![Build Status](https://travis-ci.com/Michionlion/rayterm.svg?branch=master)](https://travis-ci.com/Michionlion/rayterm)

### Overview

This repository is the GPU implementation of `RayTerm`. There is also an
incomplete CPU implementation, available [here](https://github.com/Michionlion/rayterm-cpu).

[![Gif Demo](https://media.githubusercontent.com/media/Michionlion/compdoc/master/rtexplore/terminal.gif)](https://www.youtube.com/embed/mr3k4GER3Rs?loop=1&playlist=mr3k4GER3Rs&controls=0&autoplay=1)

### Development

#### Dependencies

`RayTerm` requires the following to compile:

* [OptiX 6.0.0 SDK](http://raytracing-docs.nvidia.com/optix_6_0/index.html)
  (Installed to `lib/optix`)
* [CUDA Toolkit 10.1](https://developer.nvidia.com/cuda-toolkit-archive)
  (Installed to `lib/cuda`)
* [MDL SDK](https://developer.nvidia.com/mdl-sdk)
  (Installed to `lib/mdl`)
* [GoogleTest](https://github.com/google/googletest)
  (Installed to `lib/googletest`)
* [Tickit](https://github.com/Michionlion/libtickit)
  (Installed to `lib/libtickit`)

These libraries are all installed automatically by running `gradle deps`,
although installation may take some time as many of them are large. This
automated installation requires [git lfs](https://git-lfs.github.com/), and
will fail without it. See `gradle.properties` to change the default expected
installation locations, if you wish to use external packages.

The `gcc` toolchain is used to compile `RayTerm`. We specifically require
`g++-7`, and by default search `/usr/bin/` for it. This is taken care of
automatically when `gradle compileCu` is run, but may need to be manually
changed if the `gcc` installation changes.

When running built objects, the `rayterm` library must be loadable, as well as
`optix`, `cudart`, `libtickit`, and its internal dependencies
[`libtermkey`](http://www.leonerd.org.uk/code/libtermkey/) and
[`libunibilium`](https://github.com/mauke/unibilium) (which are not
automatically installed). The `rayterm` library can be installed with
`gradle install`, and its location can be modified in `gradle.properties`. If
`libtickit` is statically linked, then it can be left out of the above list.

Linting dependencies such as [mdl](https://github.com/markdownlint/markdownlint),
[proselint](https://github.com/amperser/proselint/), and
[clang-tidy](https://clang.llvm.org/extra/clang-tidy/) ensure documentation and
code are well written; they should be installed before running `gradle lint`.

#### Testing

For manual rendering comparisons, [blender](https://www.blender.org/) is used.

#### Build System

`RayTerm` uses the [Gradle](https://gradle.org/) to automate compilation and
testing. To build `RayTerm`, run the command `gradle build`. To run unit tests,
run `gradle test`. To lint documentation and source code (where applicable),
issue `gradle lint`. Custom scripts are also used to simplify and speed up the
development process: `test.sh` compiles only what is needed for a `ppm` render
test, and `run.sh` compiles the necessary components for an `rtexplore`
executable. Both are configurable with simple command-line arguments.
