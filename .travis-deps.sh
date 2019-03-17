#!/bin/bash

export BASE="${BASE:-$HOME}"

export GRADLE_VERSION="5.1"
export GRADLE_URL="https://services.gradle.org/distributions/gradle-$GRADLE_VERSION-bin.zip"

export GRADLE_LOCATION="$BASE/gradle-$GRADLE_VERSION"

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