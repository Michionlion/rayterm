#!/bin/bash

TYPE="$1"
if [ -z "$TYPE" ]; then
    TYPE="debug"
fi
TYPE=${TYPE,,}

test -f error.log && rm error.log

if gradle installRtexplore${TYPE^}Executable --console=rich; then

    printf "\nRunning %s executable...\n" $TYPE
    sleep 0.5

    xterm -T "i3-float" -geometry 100x50+200+150 -fa "AnonymousProMinus" -fs 8 -e ./build/install/rtexplore/$TYPE/rtexplore 2> error.log

    LOG=$(cat error.log)

    if [ -z "$LOG" ]; then
        printf "\n -- No Errors -- \n"
    else
        printf "\n -- Error Log -- \n"
        echo "$LOG"
    fi
fi
