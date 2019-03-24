#!/bin/bash

FONT="DejaVuSansMono Nerd Font Mono"
LOG="error.log"
TYPE="$1"
if [ -z "$TYPE" ]; then
    TYPE="debug"
fi
TYPE=${TYPE,,}

: > $LOG

if gradle compileCu installRtexplore${TYPE^}Executable --console=rich; then

    printf "\nRunning %s executable...\n" $TYPE
    sleep 0.5

if [[ -z "$NO_TERM" ]]; then
    LD_LIBRARY_PATH=lib/libtickit/.libs:lib/cuda/lib64:lib/optix/lib64:build/libs/rayterm/shared/$TYPE TICKIT_DEBUG_FD=4 TICKIT_DEBUG_FLAGS=U xterm -T "i3-float" -geometry 100x50+200+150 -fa "$FONT" -fs 12 -e ./build/install/rtexplore/$TYPE/rtexplore 4>&2
else
    LD_LIBRARY_PATH=lib/libtickit/.libs:lib/cuda/lib64:lib/optix/lib64:build/libs/rayterm/shared/$TYPE TICKIT_DEBUG_FD=4 TICKIT_DEBUG_FLAGS=U  ./build/install/rtexplore/$TYPE/rtexplore 2> >(tee -a $LOG >&2)
fi
    LOG_CONTENTS=$(cat $LOG)

    if [ -z "$LOG_CONTENTS" ]; then
        printf "\n -- No Errors -- \n"
    else
        printf "\n -- Error Log -- \n"
        echo "$LOG_CONTENTS"
    fi
fi
