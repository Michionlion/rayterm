#!/bin/bash

export BASE="${BASE:-$HOME}"

export GRADLE_VERSION="5.1"
export GRADLE_URL="https://services.gradle.org/distributions/gradle-$GRADLE_VERSION-bin.zip"

export GRADLE_LOCATION="$BASE/gradle-$GRADLE_VERSION"
export OPTIX_LOCATION="$BASE/optix"
export CUDA_LOCATION="$BASE/cuda"
export MDL_LOCATION="$BASE/mdl"

if [[ ! -d "$GRADLE_LOCATION" ]]; then
    (
    \cd "$BASE" || exit
    curl -L -o "gradle.zip" "$GRADLE_URL"
    unzip "gradle.zip"
    rm -rf "gradle.zip"
    )
fi

# add gradle bin to path at the beginning to ensure it overwrites old gradle
export PATH="$GRADLE_LOCATION/bin:$PATH"

# check dependency folders (for cache), and update or clone as required

if [[ -d "$OPTIX_LOCATION" ]]; then
    git -C "$OPTIX_LOCATION" pull
else
    git clone --depth=1 --branch=master https://github.com/Michionlion/optix.git "$OPTIX_LOCATION"
fi

if [[ -d "$CUDA_LOCATION" ]]; then
    git -C "$CUDA_LOCATION" pull
else
    git clone --depth=1 --branch=master https://github.com/Michionlion/cuda.git "$CUDA_LOCATION"
fi

if [[ -d "$MDL_LOCATION" ]]; then
    git -C "$MDL_LOCATION" pull
else
    git clone --depth=1 --branch=master https://github.com/Michionlion/mdl.git "$MDL_LOCATION"
fi
