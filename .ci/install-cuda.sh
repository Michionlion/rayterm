#!/bin/bash
# Credit to tmcdonell
# https://github.com/tmcdonell/travis-scripts

export CUDA=10.0.130-1

wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu1604/x86_64/cuda-repo-ubuntu1604_${CUDA}_amd64.deb
sudo apt-key adv --fetch-keys http://developer.download.nvidia.com/compute/cuda/repos/ubuntu1604/x86_64/7fa2af80.pub
sudo dpkg -i cuda-repo-ubuntu1604_${CUDA}_amd64.deb
sudo apt-get update -qq
export CUDA_VER=$(expr ${CUDA} : '\([0-9]*\.[0-9]*\)')
export CUDA_APT=${CUDA_VER/./-}
sudo apt-get install -y cuda-drivers cuda-core-${CUDA_APT} cuda-cudart-dev-${CUDA_APT}
if [ ${CUDA_INSTALL_EXTRA_LIBS:-1} -ne 0 ]; then
  if [ ${CUDA_VER%.*} -lt 7 ]; then
    sudo apt-get install -y cuda-cufft-dev-${CUDA_APT} cuda-cublas-dev-${CUDA_APT} cuda-cusparse-dev-${CUDA_APT}
  else
    sudo apt-get install -y cuda-cufft-dev-${CUDA_APT} cuda-cublas-dev-${CUDA_APT} cuda-cusparse-dev-${CUDA_APT} cuda-cusolver-dev-${CUDA_APT}
  fi
fi
travis_retry sudo apt-get clean
export CUDA_HOME=/usr/local/cuda-${CUDA_VER}
export LD_LIBRARY_PATH=${CUDA_HOME}/nvvm/lib64:${LD_LIBRARY_PATH}
export LD_LIBRARY_PATH=${CUDA_HOME}/lib64:${LD_LIBRARY_PATH}
export PATH=${CUDA_HOME}/bin:${PATH}

gcc --version
which gcc
which nvcc
