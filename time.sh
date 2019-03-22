#!/bin/bash


if gradle installRtimeReleaseExecutable --console=rich; then
    ./build/install/rtime/release/rtime "$@"
fi
