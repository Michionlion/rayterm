#!/bin/bash

wget -o optix.sh https://developer.nvidia.com/designworks/optix/downloads/6.0.0/linux64

./optix.sh --prefix $HOME/optix --exclude-subdir
