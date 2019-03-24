#!/bin/bash
# Run raytraceTest only


# parse flags
OUTPUT=false
FLAGS=""
while [ $# -gt 0 ]; do
    case $1 in
        -r|--res|--resolution|--multi)
            if [ $# -gt 1 ]; then
                FLAGS="$FLAGS-PRES_MULT=$2 "
                shift
            else
                echo -e "\033[91mNo value specified after $1!'\033[0m"
            fi
        ;;
        -s|--samples)
            if [ $# -gt 1 ]; then
                FLAGS="$FLAGS-PSAMPLES=$2 "
                shift
            else
                echo -e "\033[91mNo integer value specified after $1!'\033[0m"
            fi
        ;;
        -o|--open)
            OPEN=true
        ;;
        -np|--no-progress)
            FLAGS="$FLAGS-PNO_PROGRESS "
        ;;

        -t|--test)
            TEST=true
        ;;
        *)
            echo -e "\033[2;91mUnrecognized flag '$1!'\033[0m"
    esac
shift
done

COMPILED=1
if [[ "$FLAGS" != "" ]]; then
    gradle installRaytraceTestReleaseGoogleTestExe compileCu $FLAGS
    COMPILED=$?
    echo "Compiled with $FLAGS"
    shift
else
    gradle installRaytraceTestReleaseGoogleTestExe compileCu
    COMPILED=$?
    echo "Compiled with default samples"
fi

if  [[ "$COMPILED" != "0" ]]; then
    exit
fi

if [[ "$TEST" = "true" ]]; then
    LD_LOAD_LIBRARY=build/install/raytraceTest/release/lib gdb --exec=build/install/raytraceTest/release/lib/raytraceTest -ex "run" -ex "bt"
    exit
fi

./build/install/raytraceTest/release/raytraceTest --gtest_filter=RaytracerTest.Screenshot

if [[ "$OPEN" = "true" ]]; then
    exec feh --force-aliasing -Z test_image.ppm &
    disown
fi
