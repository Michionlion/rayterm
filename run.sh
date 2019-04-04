#!/bin/bash

trap "tput reset" SIGABRT SIGTERM

# parse flags
SELF="0"
TYPE="debug"
FLAGS=""
LOG="error.log"
FONT="DejaVuSansMono Nerd Font Mono"
FONTSIZE="9"
GEOMETRY="80x26+300+250"
while [ $# -gt 0 ]; do
    case $1 in
        -s|--samples)
            if [[ $# -gt 1 ]]; then
                FLAGS="$FLAGS--samples $2 "
                shift
            else
                echo -e "\033[91mNo integer value specified after $1!'\033[0m"
                exit
            fi
        ;;
        -f|--flags)
            if [[ $# -gt 1 ]]; then
                FLAGS="$FLAGS$2 "
                shift
            else
                echo -e "\033[91mNo string value specified after $1!'\033[0m"
                exit
            fi
        ;;
        -l|--log)
            if [[ $# -gt 1 ]]; then
                LOG="$2"
                shift
            else
                echo -e "\033[91mNo string value specified after $1!'\033[0m"
                exit
            fi
        ;;
        -ft|--font)
            if [[ $# -gt 1 ]]; then
                FONT="$2"
                shift
            else
                echo -e "\033[91mNo string value specified after $1!'\033[0m"
                exit
            fi
        ;;
        -fs|--fontsize)
            if [[ $# -gt 1 ]]; then
                FONTSIZE="$2"
                shift
            else
                echo -e "\033[91mNo integer value specified after $1!'\033[0m"
                exit
            fi
        ;;
        -g|--geometry)
            if [[ $# -gt 1 ]]; then
                GEOMETRY="$2"
                shift
            else
                echo -e "\033[91mNo geometry value specified after $1!'\033[0m"
                exit
            fi
        ;;
        --self)
            SELF="1"
        ;;
        *)
            echo -e "\033[91mUnrecognized flag '$1!'\033[0m"
            exit
    esac
shift
done

# verification and setup
TYPE=${TYPE,,}
: > "$LOG"

if gradle compileCu installRtexplore${TYPE^}Executable --console=rich; then

    printf "\nRunning %s executable...\n" "$TYPE"
    printf "Geometry: %s\nFlags: %s\n" "$GEOMETRY" "$FLAGS"
    sleep 0.5

if [[ "$SELF" = "1" ]]; then
    # shellcheck disable=SC2086
    LD_LIBRARY_PATH=build/install/rtexplore/$TYPE/lib COLORTERM=truecolor TICKIT_DEBUG_FG=4 TICKIT_DEBUG_FLAGS=U ./build/install/rtexplore/$TYPE/rtexplore $FLAGS 4> >($LOG)
else
    # shellcheck disable=SC2086
    LD_LIBRARY_PATH=build/install/rtexplore/$TYPE/lib COLORTERM=truecolor TICKIT_DEBUG_FD=4 TICKIT_DEBUG_FLAGS=U xterm -T "i3-float" -geometry "$GEOMETRY" -fa "$FONT" -fs "$FONTSIZE" -e ./build/install/rtexplore/$TYPE/rtexplore $FLAGS 4>&2
fi
    LOG_CONTENTS=$(cat "$LOG")

    if [ -z "$LOG_CONTENTS" ]; then
        printf "\n -- No Errors -- \n"
    else
        printf "\n -- Error Log -- \n"
        echo "$LOG_CONTENTS"
    fi
fi
