#!/bin/sh

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $DIR
mkdir build_xcode
cd ./build_xcode
cmake -G"Xcode" -DCMAKE_BUILD_TYPE=Debug ..