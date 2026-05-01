#!/bin/bash 

QT_VERSION=6.11.0

GT_DIR_PREFIX=$HOME/Projekt/Taurus/gen_tau_framework

export CMAKE_PREFIX_PATH=$HOME/Qt/$QT_VERSION/gcc_64

$GT_DIR_PREFIX/scripts/cmake_conf.sh -b -j 32 -T
