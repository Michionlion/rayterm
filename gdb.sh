#!/bin/bash

LD_LIBRARY_PATH=lib/libtickit/.libs:lib/cuda/lib64:lib/optix/lib64:build/libs/rayterm/shared/debug gdb ./build/exe/rtexplore/debug/rtexplore
