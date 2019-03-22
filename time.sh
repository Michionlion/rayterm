#!/bin/bash


TIMES="$1"
RES="$2"

TYPE="$3"
if [ -z "$TYPE" ]; then
    TYPE="debug"
fi
TYPE=${TYPE,,}



if gradle installRtime${TYPE^}Executable --console=rich; then

    printf "\nRunning %s executable...\n" $TYPE
    sleep 0.5

    ./build/install/rtime/$TYPE/rtime $TIMES $RES
fi
